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
#ifndef INTERPOLATER_H
#define INTERPOLATER_H

#include <array>
#include <limits>
#include <string>
#include <tuple>
#include "adcircmodules.h"
#include "cdate.h"

class Interpolate {
 public:
  struct InputOptions {
    std::string mesh, globalfile, stationfile, outputfile, coldstart, refdate;
    bool magnitude, direction, readimeds, writeimeds, readdflow, writedflow,
        readasciimesh, keepwet;
    size_t startsnap, endsnap;
    int epsg_global, epsg_station, epsg_output;
    double multiplier;
    InputOptions()
        : mesh(std::string()),
          globalfile(std::string()),
          stationfile(std::string()),
          outputfile(std::string()),
          coldstart(std::string()),
          refdate(std::string()),
          magnitude(false),
          direction(false),
          readimeds(false),
          writeimeds(false),
          readdflow(false),
          writedflow(false),
          readasciimesh(false),
          keepwet(false),
          startsnap(std::numeric_limits<size_t>::max()),
          endsnap(std::numeric_limits<size_t>::max()),
          epsg_global(4326),
          epsg_station(4326),
          epsg_output(4326),
          multiplier(1.0) {}
  };

  Interpolate(const InputOptions &options);

  void run();

 private:
  struct Weight {
    bool found;
    std::array<size_t, 3> node_index;
    std::array<double, 3> weight;
    Weight() : found(false), node_index{0, 0, 0}, weight{0.0, 0.0, 0.0} {}
  };

  Adcirc::Geometry::Mesh readMesh(const Adcirc::Output::OutputFormat &filetype);
  Adcirc::Output::Hmdf readStationLocations();
  Adcirc::Output::Hmdf readStationList();
  void writeAdcirc(Adcirc::Output::ReadOutput &output, Adcirc::Output::Hmdf &stn,
                   const std::vector<double> &time,
                   const std::vector<long> &iteration,
                   const std::vector<std::vector<double>> &v1,
                   const std::vector<std::vector<double>> &v2);
  void generateInterpolationWeights(Adcirc::Geometry::Mesh &m,
                                    Adcirc::Output::Hmdf &stn);
  double interpScalar(Adcirc::Output::ReadOutput &data, Weight &w);
  std::tuple<double, double> interpVector(Adcirc::Output::ReadOutput &data,
                                          Weight &w);
  double interpolateDryValues(double v1, double w1, double v2, double w2,
                              double v3, double w3, double defaultVal);
  Date dateFromString(const std::string &dateString);

  InputOptions m_inputOptions;
  std::vector<Weight> m_weights;
};

#endif  // INTERPOLATER_H
