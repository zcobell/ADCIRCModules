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
#include <iostream>
#include <string>
#include "cxxopts.hpp"
#include "interpolate.h"
#include "logging.h"

Interpolate::InputOptions parseCommandLineOptions(cxxopts::ParseResult &parser);
void validateOptionsList(const Interpolate::InputOptions &input);

int main(int argc, char *argv[]) {
  cxxopts::Options options("interpolateAdcircStations",
                           "ADCIRC Global Data Interpolation");

  // clang-format off
  options.add_options() ("h,help", "show this message")
                     ("m,mesh", "specify the ADCIRC geometry used in the global file",cxxopts::value<std::string>())
                     ("g,global","specify the global ADCIRC output",cxxopts::value<std::string>())
                     ("s,station", "specify the station locations to be used",cxxopts::value<std::string>())
                     ("o,output", "specify the output file to be created",cxxopts::value<std::string>())
                     ("magnitude","write out the magnitude of vector quantities")
                     ("direction","write out the direction of vector quantities")
                     ("readimeds", "read station data as IMEDS instead of xyz")
                     ("writeimeds", "write station data as IMEDS instead of fort.61")
                     ("readDFlow", "read a DFlow-FM polyline file for station locations")
                     ("writeDFlow","write DFlow-FM boundary conditions files")
                     ("coldstart","date/time of the simulation cold start for time reference in IMEDS files. Ex: yyyyMMddhhmmss",cxxopts::value<std::string>())
                     ("refdate","date/time reference used when writing input files for DFlow-FM. Ex: Ex: yyyyMMddhhmmss",cxxopts::value<std::string>())
                     ("multiplier","value multiplied to the output value",cxxopts::value<double>())
                     ("start_snap","start reading netCDF output at specified snap",cxxopts::value<int>())
                     ("end_snap","stop reading netCDF output at specified snap",cxxopts::value<int>())
                     ("keepwet", "If a point goes dry, use the previous wet value")
                     ("readAsciiMesh", "Force the code to read an ascii meshfile instead of defaulting to the NetCDF ADCIRC output file data")
                     ("epsg_global","Specify the coordinate system of the global time series file (default: 4326)",cxxopts::value<int>())
                     ("epsg_station","Specify the coordinate system of the input station locations (default: 4326)",cxxopts::value<int>())
                     ("epsg_output","Specify the coordinate system of the output data file (default: 4326)",cxxopts::value<int>());
  // clang-format on

  if (argc == 1) {
    Adcirc::Logging::logError("No options specified");
    std::cout << options.help() << std::endl;
    return 1;
  }

  auto result = options.parse(argc, argv);

  if (result["help"].count() > 0) {
    std::cout << options.help();
    return 1;
  }

  Interpolate::InputOptions input = parseCommandLineOptions(result);
  validateOptionsList(input);

  Interpolate interp(input);
  interp.run();

  return 0;
}

Interpolate::InputOptions parseCommandLineOptions(
    cxxopts::ParseResult &parser) {
  Interpolate::InputOptions input;
  if (parser["mesh"].count() > 0) input.mesh = parser["mesh"].as<std::string>();
  if (parser["global"].count() > 0)
    input.globalfile = parser["global"].as<std::string>();
  if (parser["station"].count() > 0)
    input.stationfile = parser["station"].as<std::string>();
  if (parser["output"].count() > 0)
    input.outputfile = parser["output"].as<std::string>();
  if (parser["magnitude"].count() > 0)
    input.magnitude = parser["magnitude"].as<bool>();
  if (parser["direction"].count() > 0)
    input.direction = parser["direction"].as<bool>();
  if (parser["readimeds"].count() > 0)
    input.readimeds = parser["readimeds"].as<bool>();
  if (parser["writeimeds"].count() > 0)
    input.writeimeds = parser["writeimeds"].as<bool>();
  if (parser["readDFlow"].count() > 0)
    input.readdflow = parser["readDFlow"].as<bool>();
  if (parser["writeDFlow"].count() > 0)
    input.writedflow = parser["writeDFlow"].as<bool>();
  if (parser["coldstart"].count() > 0)
    input.coldstart = parser["coldstart"].as<std::string>();
  if (parser["refdate"].count() > 0)
    input.refdate = parser["refdate"].as<std::string>();
  if (parser["multiplier"].count() > 0)
    input.multiplier = parser["multiplier"].as<double>();
  if (parser["start_snap"].count() > 0)
    input.startsnap = parser["start_snap"].as<int>();
  if (parser["end_snap"].count() > 0)
    input.endsnap = parser["end_snap"].as<int>();
  if (parser["epsg_global"].count() > 0)
    input.epsg_global = parser["epsg_global"].as<int>();
  if (parser["epsg_output"].count() > 0)
    input.epsg_output = parser["epsg_output"].as<int>();
  if (parser["epsg_station"].count() > 0)
    input.epsg_station = parser["epsg_station"].as<int>();
  return input;
}

void validateOptionsList(const Interpolate::InputOptions &input) {
  if (input.mesh == std::string()) {
    Adcirc::Logging::logError("No mesh supplied");
    exit(1);
  }
  if (input.globalfile == std::string()) {
    Adcirc::Logging::logError("No global data supplied");
    exit(1);
  }
  if (input.outputfile == std::string()) {
    Adcirc::Logging::logError("No output file specified");
    exit(1);
  }
  if (input.stationfile == std::string()) {
    Adcirc::Logging::logError("No station file supplied");
    exit(1);
  }
  if (input.writeimeds && input.coldstart == std::string()) {
    Adcirc::Logging::logError(
        "Must supply cold start date to write imeds format files.");
    exit(1);
  }
  if (input.writedflow && input.coldstart == std::string()) {
    Adcirc::Logging::logError(
        "Must supply cold start date to write dflow format files.");
    exit(1);
  }
  if (input.writedflow && input.refdate == std::string()) {
    Adcirc::Logging::logError(
        "Must supply reference date to write dflow format files.");
    exit(1);
  }
  return;
}
