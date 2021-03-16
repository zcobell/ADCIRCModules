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

#include "OceanweatherRecord.h"

#include <cassert>

#include "Constants.h"
#include "Logging.h"
#include "boost/format.hpp"
#include "boost/spirit/include/phoenix.hpp"
#include "boost/spirit/include/qi.hpp"

using namespace Adcirc;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

OceanweatherRecord::OceanweatherRecord()
    : m_backgroundPressure(1013.0), m_gridchanged(true) {}

void OceanweatherRecord::addDomain(std::ifstream *fid_pressure,
                                   std::ifstream *fid_wind) {
  Domain d;
  d.fid_pressure = fid_pressure;
  d.fid_wind = fid_wind;
  this->m_domains.push_back(d);
}

int OceanweatherRecord::read() {
  int has_error = 0;
  for (auto &d : m_domains) {
    if (d.fid_pressure->peek() && d.fid_wind->peek()) {
      std::string header_pressure, header_wind;
      std::getline(*(d.fid_pressure), header_pressure);
      std::getline(*(d.fid_wind), header_wind);

      if (header_pressure.length() < 68) return 1;
      if (header_wind.length() < 68) return 1;

      Grid p = parseOwiGridLine(header_pressure);
      Grid w = parseOwiGridLine(header_wind);
      if (p != w) {
        adcircmodules_throw_exception(
            "Grids between pressure and wind fields are not identical");
      }

      if (p != d.grid) {
        m_gridchanged = true;
        d.grid = std::move(p);
      } else {
        m_gridchanged = false;
      }

      int ierr =
          OceanweatherRecord::readData(d, d.fid_pressure, d.data_pressure);
      ierr += OceanweatherRecord::readData(d, d.fid_wind, d.data_u);
      ierr += OceanweatherRecord::readData(d, d.fid_wind, d.data_v);
      if (ierr != 0) has_error = 1;
    } else {
      Logging::warning("Reached end of file");
      has_error = 1;
    }
  }
  return has_error;
}

size_t OceanweatherRecord::ndomain() const { return this->m_domains.size(); }

OceanweatherRecord::Domain *OceanweatherRecord::domain(size_t index) {
  assert(index < ndomain());
  return &m_domains[index];
}

std::tuple<double, double, double, size_t> OceanweatherRecord::get(
    const size_t i, const size_t j, const size_t domain) const {
  const auto idx = m_domains[domain].grid.index2DtoIndex1D(i, j);
  return idx < m_domains[domain].data_pressure.size()
             ? std::make_tuple(m_domains[domain].data_pressure[idx],
                               m_domains[domain].data_u[idx],
                               m_domains[domain].data_v[idx], domain)
             : std::make_tuple(backgroundPressure(), 0.0, 0.0,
                               std::numeric_limits<size_t>::max());
}

std::tuple<double, double, double, size_t> OceanweatherRecord::get(
    const double x, const double y, size_t domain) const {
  if (domain == std::numeric_limits<size_t>::max()) {
    domain = findDomain(x, y);
    if (domain == std::numeric_limits<size_t>::max()) {
      return {backgroundPressure(), 0.0, 0.0, domain};
    }
  }

  size_t i, j;
  double w1, w2, w3, w4;
  std::tie(i, j, domain, w1, w2, w3, w4) = interpolationWeight(x, y, domain);

  double p1, u1, v1;
  std::tie(p1, u1, v1, std::ignore) = this->get(i, j, domain);

  double p2, u2, v2;
  std::tie(p2, u2, v2, std::ignore) = this->get(i + 1, j, domain);

  double p3, u3, v3;
  std::tie(p3, u3, v3, std::ignore) = this->get(i + 1, j + 1, domain);

  double p4, u4, v4;
  std::tie(p4, u4, v4, std::ignore) = this->get(i + 1, j, domain);

  double pv = w1 * p1 + w2 * p2 + w3 * p3 + w4 * p4;
  double uv = w1 * u1 + w2 * u2 + w3 * u3 + w4 * u4;
  double vv = w1 * v1 + w2 * v2 + w3 * v3 + w4 * v4;
  return {pv, uv, vv, domain};
}

std::tuple<size_t, size_t, size_t, double, double, double, double>
OceanweatherRecord::interpolationWeight(const double x, const double y,
                                        size_t domain) const {
  if (domain == std::numeric_limits<size_t>::max()) {
    domain = this->findDomain(x, y);
    if (domain == std::numeric_limits<size_t>::max()) {
      return {0.0, 0.0, domain, 0.0, 0.0, 0.0, 0.0};
    }
  }
  if (inDomain(domain, x, y)) {
    int xi = std::floor((x - m_domains[domain].grid.xmin) /
                        m_domains[domain].grid.dx);
    int yj = std::floor((y - m_domains[domain].grid.ymin) /
                        m_domains[domain].grid.dy);
    double x1 = m_domains[domain].grid.x(xi);
    double x2 = m_domains[domain].grid.x(xi + 1);
    double y1 = m_domains[domain].grid.y(yj);
    double y2 = m_domains[domain].grid.y(yj + 1);
    double w = (x2 - x1) * (y2 - y1);
    double w1 = ((x2 - x) * (y2 - y)) / w;
    double w2 = ((x - x1) * (y2 - y)) / w;
    double w3 = ((x - x1) * (y - y1)) / w;
    double w4 = ((x2 - x) * (y - y1)) / w;
    return {xi, yj, domain, w1, w2, w3, w4};
  } else {
    return {0.0, 0.0, domain, 0.0, 0.0, 0.0, 0.0};
  }
}

bool OceanweatherRecord::inDomain(size_t domain, const double x,
                                  const double y) const {
  return x >= m_domains[domain].grid.xmin && x < m_domains[domain].grid.xmax &&
         y >= m_domains[domain].grid.ymin && y < m_domains[domain].grid.ymax;
}

size_t OceanweatherRecord::findDomain(const double x, const double y) const {
  for (auto i = ndomain() - 1; i >= 0; --i) {
    if (this->inDomain(i, x, y)) {
      return i;
    }
  }
  return std::numeric_limits<size_t>::max();
}

bool OceanweatherRecord::gridChanged() const { return m_gridchanged; }

double OceanweatherRecord::backgroundPressure() const {
  return m_backgroundPressure;
}

void OceanweatherRecord::setBackgroundPressure(double backgroundPressure) {
  m_backgroundPressure = backgroundPressure;
}

int OceanweatherRecord::readData(const OceanweatherRecord::Domain &d,
                                 std::ifstream *fid,
                                 std::vector<double> &array) {
  constexpr size_t rpl = 8;  // 8 records per line for OWI format
  const size_t nv = d.grid.nx * d.grid.ny;

  array.clear();
  array.reserve(nv);

  for (size_t i = 0; i < nv; i += rpl) {
    std::string line;
    std::getline(*(fid), line);
    auto err = qi::phrase_parse(
        line.begin(), line.end(),
        (*(qi::double_[phoenix::push_back(phoenix::ref(array), qi::_1)])),
        ascii::space);
  }
  return 0;
}

OceanweatherRecord::Grid OceanweatherRecord::parseOwiGridLine(
    const std::string &gridline) {
  std::string ilat, ilon, dx, dy, swlat, swlon, dt;
  ilat = gridline.substr(5, 4);
  ilon = gridline.substr(15, 4);
  dx = gridline.substr(22, 6);
  dy = gridline.substr(31, 6);
  swlat = gridline.substr(43, 8);
  swlon = gridline.substr(57, 8);
  dt = gridline.substr(68, 12);

  Grid g;
  g.nx = stoi(ilon);
  g.ny = stoi(ilat);
  g.dx = stod(dx);
  g.dy = stod(dy);
  g.xmin = stod(swlon);
  g.ymin = stod(swlat);
  g.xmax = g.x(g.nx - 1);
  g.ymax = g.y(g.ny - 1);
  g.time =
      CDate(stoi(dt.substr(0, 4)), stoi(dt.substr(4, 2)), stoi(dt.substr(6, 2)),
            stoi(dt.substr(8, 2)), stoi(dt.substr(10, 2)), 0);

  return g;
}

Adcirc::OceanweatherTrackInfo OceanweatherRecord::get_current_storm_info()
    const {
  OceanweatherTrackInfo info;

  for (const auto &d : m_domains) {
    for (auto i = 0; i < d.grid.nx * d.grid.ny; ++i) {
      size_t ii, jj;
      std::tie(ii, jj) = d.grid.index1DtoIndex2D(i);
      double vel = Constants::magnitude(d.data_u[i], d.data_v[i]);
      double cp = d.data_pressure[i];
      if (cp < info.min_central_pressure()) {
        info.set_min_central_pressure(cp);
        info.set_storm_center(Point(d.grid.x(ii), d.grid.y(jj)));
      }
      if (vel > info.maximum_wind_velocity()) {
        info.set_maximum_wind_velocity(vel);
        info.set_max_velocity_location(Point(d.grid.x(ii), d.grid.y(jj)));
      }
    }
  }

  info.set_radius_to_max_winds(
      Constants::distance(info.storm_center().x(), info.storm_center().y(),
                          info.max_velocity_location().x(),
                          info.max_velocity_location().y(), true));

  return info;
}

Adcirc::CDate OceanweatherRecord::current_time() const {
  return m_domains[0].grid.time;
}
