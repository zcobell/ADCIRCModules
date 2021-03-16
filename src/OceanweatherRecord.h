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
#ifndef OCEANWEATHERRECORD_H
#define OCEANWEATHERRECORD_H

#include <array>
#include <fstream>
#include <vector>

#include "CDate.h"
#include "OceanweatherTrackInfo.h"

namespace Adcirc {

class OceanweatherRecord {
 public:
  OceanweatherRecord();

  void addDomain(std::ifstream *fid_pressure, std::ifstream *fid_wind);

  int read();

  struct Grid {
    double xmin;
    double ymin;
    double xmax;
    double ymax;
    double dx;
    double dy;
    size_t nx;
    size_t ny;
    Adcirc::CDate time;

    double x(size_t i) const { return i < nx ? xmin + i * dx : 0.0; }
    double y(size_t j) const { return j < ny ? ymin + j * dy : 0.0; }

    size_t index2DtoIndex1D(size_t i, size_t j) const { return nx * j + i; }

    std::pair<size_t, size_t> index1DtoIndex2D(size_t i) const {
      return {i % nx, i / nx};
    }

    bool operator==(const Grid &g) const {
      if (xmin != g.xmin) return false;
      if (xmax != g.xmax) return false;
      if (ymin != g.ymin) return false;
      if (ymax != g.ymax) return false;
      if (dx != g.dx) return false;
      if (dy != g.dy) return false;
      if (nx != g.nx) return false;
      if (ny != g.ny) return false;
      if (time != g.time) return false;
      return true;
    }

    bool operator!=(const Grid &g) { return !(*(this) == g); }
  };

  struct Domain {
    std::ifstream *fid_pressure;
    std::ifstream *fid_wind;
    std::vector<double> data_pressure;
    std::vector<double> data_u;
    std::vector<double> data_v;
    Grid grid;
  };

  Adcirc::OceanweatherTrackInfo get_current_storm_info() const;

  size_t ndomain() const;

  Domain *domain(size_t index);

  std::tuple<double, double, double, size_t> get(size_t i, size_t j,
                                                 size_t domain) const;

  std::tuple<double, double, double, size_t> get(
      double x, double y,
      size_t domain = std::numeric_limits<size_t>::max()) const;

  std::tuple<size_t, size_t, size_t, double, double, double, double>
  interpolationWeight(double x, double y,
                      size_t domain = std::numeric_limits<size_t>::max()) const;

  double backgroundPressure() const;
  void setBackgroundPressure(double backgroundPressure);

  bool gridChanged() const;

  Adcirc::CDate current_time() const;

 private:
  static OceanweatherRecord::Grid parseOwiGridLine(const std::string &gridline);
  static int readData(const Domain &d, std::ifstream *fid,
                      std::vector<double> &array);
  bool inDomain(size_t domain, double x, double y) const;
  size_t findDomain(double x, double y) const;

  std::vector<Domain> m_domains{};
  double m_backgroundPressure;
  bool m_gridchanged;
};

}  // namespace Adcirc

#endif  // OCEANWEATHERRECORD_H
