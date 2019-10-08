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
#ifndef NETCDFTIMESERIES_H
#define NETCDFTIMESERIES_H

#include "hmdf.h"
#include <string>
#include <vector>

class NetcdfTimeseries  {
 public:
  explicit NetcdfTimeseries();

  int read();

  int toHmdf(Hmdf *hmdf);

  std::string filename() const;
  void setFilename(const std::string &filename);

  int epsg() const;
  void setEpsg(int epsg);

  static int getEpsg(const std::string &file);

private:
  std::string m_filename;
  std::string m_units;
  std::string m_verticalDatum;
  std::string m_horizontalProjection;
  int m_epsg;
  size_t m_numStations;

  std::vector<double> m_fillValue;
  std::vector<double> m_xcoor;
  std::vector<double> m_ycoor;
  std::vector<size_t> m_stationLength;
  std::vector<std::string> m_stationName;
  std::vector<std::vector<long long> > m_time;
  std::vector<std::vector<double> > m_data;
};

#endif  // NETCDFTIMESERIES_H
