/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
//
// ADCIRCModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ADCIRCModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------*/

#include "GriddataWindRoughness.h"

#include "Constants.h"

using namespace Adcirc::Private;

GriddataWindRoughness::GriddataWindRoughness(
    const Adcirc::Raster::Rasterdata *raster,
    const GriddataAttribute *attribute, const GriddataConfig *config)
    : GriddataMethod(raster, attribute, config) {}

// This is a very fast approximation to the exp function. But it is only an
// approximation. exp is ~15% of the computational time required when computing
// directional wind reduction, so you can get some gains. The results are
// different but not substantially
template <typename T>
T fast_exp(const T x) noexcept {
  if (std::is_same<T, float>()) {
    constexpr auto v0 = double((1 << 20) / M_LN2);
    constexpr auto v1 = double((1 << 20) * 1023 - 0);
    union union_exp {
      double d_;
      int32_t i_[2];
      explicit union_exp(int32_t v) : i_{0, v} {}
    };
    union_exp uu(v0 * x + v1);
    return uu.d_;
  } else if (std::is_same<T, double>()) {
    constexpr auto v0 = double((int64_t(1) << 52) / M_LN2);
    constexpr auto v1 = double((int64_t(1) << 52) * 1023 - 0);
    union union_exp {
      double d_;
      int64_t i_;
      explicit union_exp(int64_t v) : i_{v} {}
    };
    union_exp uu(v0 * x + v1);
    return uu.d_;
  } else {
    return T(0);
  }
}
#ifdef ADCMOD_USE_FAST_MATH
#define griddata_exp fast_exp
#else
#define griddata_exp std::exp
#endif

//...A couple constants used within
constexpr static double c_rootTwoPi() { return 2.50662827463100024161; }
constexpr static double c_oneOver2MinusRoot3() {
  return 1.0 / (2.0 - Adcirc::Constants::root3());
}
constexpr static double c_oneOver2PlusRoot3() {
  return 1.0 / (2.0 + Adcirc::Constants::root3());
}
constexpr static double c_epsilonSquared() {
  return std::numeric_limits<double>::epsilon() *
         std::numeric_limits<double>::epsilon();
}
constexpr static double c_oneOverWindSigmaSquared() {
  return 1.0 / (GriddataWindRoughness::windSigma() *
                GriddataWindRoughness::windSigma());
}
constexpr static double c_windSigmaRootTwoPi() {
  return GriddataWindRoughness::windSigma() * c_rootTwoPi();
}

constexpr static std::array<std::array<char, 7>, 3> c_windDirectionLookup = {
    {{{3, 2, 1, 0, 11, 10, 9}},
     {{3, -1, -1, -1, -1, -1, 9}},
     {{3, 4, 5, 6, 7, 8, 9}}}};

std::vector<double> GriddataWindRoughness::computeMultipleFromRaster() const {
  if (attribute()->interpolationFlag() == Interpolation::NoMethod) {
    return std::vector<double>(12, config()->defaultValue());
  }

  double nearWeight = 0.0;
  std::vector<double> wind(12, 0.0);
  std::array<double, 12> weight = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                   0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  auto values = this->pixelDataInSpecifiedRadius<double>(
      GriddataWindRoughness::windRadius());

  for (const auto &v : values.pixels()) {
    if (v.valid()) {
      double w = 0.0;
      char dir = 0;
      bool found = false;
      std::tie(found, w, dir) =
          GriddataWindRoughness::computeWindDirectionAndWeight(
              attribute()->point(), v.location());
      if (found) {
        weight[dir] += w;
        wind[dir] += w * v.value();
      } else {
        nearWeight += w;
      }
    }
  }

  Adcirc::Private::GriddataWindRoughness::computeWeightedDirectionalWindValues(
      weight, wind, nearWeight);
  return wind;
}

std::vector<double> GriddataWindRoughness::computeMultipleFromLookup() const {
  if (attribute()->interpolationFlag() == Interpolation::NoMethod) {
    return std::vector<double>(12, 0.0);
  }

  double nearWeight = 0.0;
  std::vector<double> wind(12, 0.0);
  std::array<double, 12> weight = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
                                   0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

  auto values = this->pixelDataInSpecifiedRadius<int>(
      GriddataWindRoughness::windRadius());

  for (auto &v : values.pixels()) {
    if (v.valid()) {
      double zl = config()->getKeyValue(v.value());
      if (zl != config()->defaultValue()) {
        double w = 0.0;
        char dir = 0;
        bool found = false;
        std::tie(found, w, dir) =
            GriddataWindRoughness::computeWindDirectionAndWeight(
                attribute()->point(), v.location());
        if (found) {
          weight[dir] += w;
          wind[dir] += w * zl;
        } else {
          nearWeight += w;
        }
      }
    }
  }
  GriddataWindRoughness::computeWeightedDirectionalWindValues(weight, wind,
                                                              nearWeight);
  return wind;
}

double GriddataWindRoughness::gaussian(const double distance) {
  return (1.0 / (GriddataWindRoughness::windSigma() * c_rootTwoPi())) *
         griddata_exp(-distance / (2.0 * GriddataWindRoughness::windSigma() *
                                   GriddataWindRoughness::windSigma()));
}

std::tuple<bool, double, char>
GriddataWindRoughness::computeWindDirectionAndWeight(const Point &p1,
                                                     const Point &p2) {
  const double dx = (p2.x() - p1.x()) * GriddataWindRoughness::distanceFactor();
  const double dy = (p2.y() - p1.y()) * GriddataWindRoughness::distanceFactor();
  const double d = dx * dx + dy * dy;

  double w = gaussian(d);
  if (d > c_epsilonSquared()) {
    const double tanxy = std::abs(dx) > std::numeric_limits<double>::epsilon()
                             ? std::abs(dy / dx)
                             : 10000000.0;
    const int k =
        std::min(1, static_cast<int>(tanxy * c_oneOver2MinusRoot3())) +
        std::min(1, static_cast<int>(tanxy)) +
        std::min(1, static_cast<int>(tanxy * c_oneOver2PlusRoot3()));
    const auto a = static_cast<char>(sgn(dx) + 1);
    const auto b = static_cast<char>(k * sgn(dy) + 3);
    char dir = c_windDirectionLookup[a][b];
    return std::make_tuple(true, w, dir);
  } else {
    return std::make_tuple(false, w, 0);
  }
}

void GriddataWindRoughness::computeWeightedDirectionalWindValues(
    const std::array<double, 12> &weight, std::vector<double> &wind,
    double nearWeight) {
  for (size_t i = 0; i < 12; ++i) {
    double w = weight[i] + nearWeight;
    if (w > 1e-12) {
      wind[i] = wind[i] / w;
    } else {
      wind[i] = 0.0;
    }
  }
}