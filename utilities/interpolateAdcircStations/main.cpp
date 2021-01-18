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

#include "AdcircModules.h"
#include "Hmdf.h"
#include "Logging.h"
#include "StationInterpolation.h"
#include "StringConversion.h"
#include "boost/format.hpp"
#include "cxxopts.hpp"

Adcirc::Output::StationInterpolationOptions parseCommandLineOptions(
    cxxopts::ParseResult &parser);
void validateOptionsList(Adcirc::Output::StationInterpolationOptions &input);
std::vector<double> readPositiveDirectionFile(const std::string &filename);

int main(int argc, char *argv[]) {
  cxxopts::Options options("interpolateAdcircStations",
                           "ADCIRC Global Data Interpolation");

  // clang-format off
  options.add_options() ("h,help", "show this message")
                     ("v,version","prints the adcircmodules version this code is based on")
                     ("m,mesh", "specify the ADCIRC geometry used in the global file",cxxopts::value<std::string>())
                     ("g,global","specify the global ADCIRC output",cxxopts::value<std::string>())
                     ("s,station", "specify the station locations to be used",cxxopts::value<std::string>())
                     ("o,output", "specify the output file to be created",cxxopts::value<std::string>())
                     ("magnitude","write out the magnitude of vector quantities")
                     ("direction","write out the direction of vector quantities")
                     ("positive_direction","use this direction (in degrees) to denote flood and ebb tide when extracting currents. "
                                            "A single value will be used for all stations or specify a file with values for each location. "
                                            "North = 90 degrees.",cxxopts::value<std::string>())
                     ("coldstart","date/time of the simulation cold start for time reference in IMEDS files. Ex: yyyyMMddhhmmss",cxxopts::value<std::string>())
                     ("multiplier","value multiplied to the output value",cxxopts::value<double>())
                     ("start_snap","start reading netCDF output at specified snap",cxxopts::value<unsigned long long>())
                     ("end_snap","stop reading netCDF output at specified snap",cxxopts::value<unsigned long long>())
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

  Adcirc::Output::StationInterpolationOptions input =
      parseCommandLineOptions(result);

  if (result["version"].count() > 0) {
    std::cout << Adcirc::version() << std::endl;
    return 0;
  }

  validateOptionsList(input);

  Adcirc::Output::StationInterpolation interp(input);
  interp.run();

  return 0;
}

Adcirc::Output::StationInterpolationOptions parseCommandLineOptions(
    cxxopts::ParseResult &parser) {
  Adcirc::Output::StationInterpolationOptions input;
  if (parser["mesh"].count() > 0)
    input.setMesh(parser["mesh"].as<std::string>());
  if (parser["global"].count() > 0)
    input.setGlobalfile(parser["global"].as<std::string>());
  if (parser["station"].count() > 0) {
    input.setStationfile(parser["station"].as<std::string>());
    input.readStations();
  }
  if (parser["output"].count() > 0)
    input.setOutputfile(parser["output"].as<std::string>());
  if (parser["magnitude"].count() > 0)
    input.setMagnitude(parser["magnitude"].as<bool>());
  if (parser["direction"].count() > 0)
    input.setDirection(parser["direction"].as<bool>());
  if (parser["coldstart"].count() > 0)
    input.setColdstart(parser["coldstart"].as<std::string>());
  if (parser["multiplier"].count() > 0)
    input.setMultiplier(parser["multiplier"].as<double>());
  if (parser["start_snap"].count() > 0)
    input.setStartsnap(parser["start_snap"].as<unsigned long long>());
  if (parser["end_snap"].count() > 0)
    input.setEndsnap(parser["end_snap"].as<unsigned long long>());
  if (parser["epsg_global"].count() > 0)
    input.setEpsgGlobal(parser["epsg_global"].as<int>());
  if (parser["epsg_output"].count() > 0)
    input.setEpsgOutput(parser["epsg_output"].as<int>());
  if (parser["epsg_station"].count() > 0)
    input.setEpsgStation(parser["epsg_station"].as<int>());
  if (parser["positive_direction"].count() > 0) {
    std::string positive_direction_string =
        parser["positive_direction"].as<std::string>();
    bool ok;
    double pd =
        Adcirc::StringConversion::stringToDouble(positive_direction_string, ok);
    if (ok) {
      input.setPositiveDirections(pd);
    } else {
      input.readPositiveDirection(positive_direction_string);
    }
  }
  return input;
}

void validateOptionsList(Adcirc::Output::StationInterpolationOptions &input) {
  if (input.readasciimesh() ||
      Adcirc::FileIO::Generic::getFileExtension(input.globalfile()) != ".nc") {
    if (input.mesh() == std::string()) {
      Adcirc::Logging::logError("No mesh supplied", "[ERROR]: ");
      exit(1);
    }
  }
  if (input.globalfile() == std::string()) {
    Adcirc::Logging::logError("No global data supplied", "[ERROR]: ");
    exit(1);
  }
  if (input.outputfile() == std::string()) {
    Adcirc::Logging::logError("No output file specified", "[ERROR]: ");
    exit(1);
  }
  if (input.stationfile() == std::string()) {
    Adcirc::Logging::logError("No station file supplied", "[ERROR]: ");
    exit(1);
  }

  Adcirc::Output::Hmdf::HmdfFileType ft =
      Adcirc::Output::Hmdf::getFiletype(input.outputfile());
  bool needsDate = ft == Adcirc::Output::Hmdf::HmdfImeds ||
                   ft == Adcirc::Output::Hmdf::HmdfCsv ||
                   ft == Adcirc::Output::Hmdf::HmdfNetCdf;
  if (needsDate && input.coldstart() == std::string()) {
    Adcirc::Logging::logError(
        "Must supply cold start date to write the specified file format.",
        "[ERROR]: ");
    exit(1);
  }

  if (!Adcirc::FileIO::Generic::fileExists(input.globalfile())) {
    Adcirc::Logging::logError("Global file does not exist.", "[ERROR]: ");
    exit(1);
  }
  if (input.mesh() != std::string()) {
    if (!Adcirc::FileIO::Generic::fileExists(input.mesh())) {
      Adcirc::Logging::logError("Mesh file does not exist.", "[ERROR]: ");
      exit(1);
    }
  }
  if (input.hasPositiveDirection()) {
    if (!input.magnitude()) {
      Adcirc::Logging::logError(
          "Option --positive_direction must be used with --magnitude only",
          "[ERROR]: ");
      exit(1);
    }
  }

  Adcirc::Output::ReadOutput gbl(input.globalfile());
  gbl.open();
  size_t ns = gbl.numSnaps();
  gbl.close();

  if (input.startsnap() < std::numeric_limits<size_t>::max() ||
      input.startsnap() == 0) {
    if (Adcirc::FileIO::Generic::getFileExtension(input.globalfile()) !=
        ".nc") {
      Adcirc::Logging::logError("Must use netCDF format to dictate start snap.",
                                "[ERROR]: ");
      exit(1);
    }
  } else {
    input.setStartsnap(1);
  }
  if (input.endsnap() < std::numeric_limits<size_t>::max()) {
    if (input.endsnap() > gbl.numSnaps()) {
      Adcirc::Logging::logError("Specified end snap exceeds file length",
                                "[ERROR]: ");
      exit(1);
    }
  } else {
    input.setEndsnap(ns);
  }

  bool canWriteVector = ft == Adcirc::Output::Hmdf::HmdfCsv ||
                        ft == Adcirc::Output::Hmdf::HmdfAdcirc;
  if (!canWriteVector && gbl.metadata()->isVector() && !input.magnitude() &&
      !input.direction()) {
    Adcirc::Logging::logError("Specified file type cannot write vector data",
                              "[ERROR]: ");
    exit(1);
  }

  return;
}
