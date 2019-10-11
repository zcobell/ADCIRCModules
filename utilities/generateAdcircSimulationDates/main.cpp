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
#include "adcircmodules.h"
#include "boost/format.hpp"
#include "cdate.h"
#include "cxxopts.hpp"
#include "logging.h"

std::string toTidefacString(Date &d);
std::string toSwanString(Date &d);
std::tuple<Date, Date, int> readOceanweatherDates(const std::string &filename);
Date dateFromOWIString(const std::string &str);

int main(int argc, char *argv[]) {
  cxxopts::Options options("generateAdcircSimulationDates",
                           "Simulation dates from OWI files");

  // clang-format off
    options.add_options() ("h,help", "show this message")
                       ("f,filename","specify the Oceanweather file",cxxopts::value<std::string>())
                       ("s,spinup","spinup period for model before starting winds (days)",cxxopts::value<double>())
                       ("d,spindown","spindown period after wind data ends (days)",cxxopts::value<double>())
                       ("swandt","Swan model time step. Ensures end dates are a multiple of the swan time step",cxxopts::value<unsigned>());
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

  if (result["filename"].count() == 0) {
    Adcirc::Logging::logError("No Oceanweather file specified.");
    return 1;
  }

  std::string owifile = result["filename"].as<std::string>();
  double spinup_days, spindown_days;
  if (result["spinup"].count() > 0) {
    spinup_days = result["spinup"].as<double>();
  } else {
    spinup_days = 0.0;
  }

  if (result["spindown"].count() > 0) {
    spindown_days = result["spindown"].as<double>();
  } else {
    spindown_days = 0.0;
  }

  if (!Adcirc::FileIO::Generic::fileExists(owifile)) {
    Adcirc::Logging::logError("Oceanweather file does not exist", "[ERROR]: ");
    return 1;
  }

  Date startWindDate, endWindDate;
  int owiStep;
  std::tie(startWindDate, endWindDate, owiStep) =
      readOceanweatherDates(owifile);

  Date startSimDate = startWindDate;
  startSimDate.add(-std::floor(spinup_days * 86400.0));
  Date endSimDate = endWindDate;
  endSimDate.add(std::floor(spindown_days * 86400.0));
  double duration =
      (endSimDate.toSeconds() - startSimDate.toSeconds()) / 86400.0;
  double windDuration =
      (endWindDate.toSeconds() - startWindDate.toSeconds()) / 86400.0;

  if (result["swandt"].count() > 0) {
    unsigned swandt = result["swandt"].as<unsigned>();
    long long swancheck =
        (endSimDate.toSeconds() - startWindDate.toSeconds()) % swandt;
    if (swancheck != 0) {
      Adcirc::Logging::warning(
          "Adding time to ensure end date falls on a SWAN cycle", "[WARNING]: ");
      endSimDate.add(swandt - swancheck);
    }
  }

  // clang-format off
  std::cout << "SIMULATION START DATE: " << startSimDate.toString() << std::endl;
  std::cout << "              TideFac Format: " << toTidefacString(startSimDate) << std::endl;
  std::cout << "                 SWAN Format: " << toSwanString(startSimDate) << std::endl << std::endl;
  std::cout << "WIND START DATE:       " << startWindDate.toString() << std::endl;
  std::cout << "              TideFac Format: " << toTidefacString(startWindDate) << std::endl;
  std::cout << "                 SWAN Format: " << toSwanString(startWindDate) << std::endl<<std::endl;
  std::cout << "WIND END DATE:         " << endWindDate.toString() << std::endl;
  std::cout << "              TideFac Format: " << toTidefacString(endWindDate) << std::endl;
  std::cout << "                 SWAN Format: " << toSwanString(endWindDate) << std::endl;
  std::cout << "SIMULATION END DATE:   " << endSimDate.toString() << std::endl;
  std::cout << "              TideFac Format: " << toTidefacString(endSimDate) << std::endl;
  std::cout << "                 SWAN Format: " << toSwanString(endSimDate) << std::endl<<std::endl;
  std::cout << "ADDITIONAL INFO:" << std::endl;
  std::cout << "        Wind Duration (days): " << windDuration << std::endl;
  std::cout << "  Simulation Duration (days): " << duration << std::endl;
  std::cout << "               Wind dt (sec): " << owiStep << std::endl;
  // clang-format on

  return 0;
}

std::tuple<Date, Date, int> readOceanweatherDates(const std::string &owifile) {
  std::ifstream owi(owifile);
  std::string line;
  std::getline(owi, line);
  std::vector<std::string> split;
  Adcirc::FileIO::Generic::splitString(line, split);
  std::string dateStartString = split[split.size() - 2];
  std::string dateEndString = split.back();

  Date dateStart = dateFromOWIString(dateStartString);
  Date dateEnd = dateFromOWIString(dateEndString);

  std::getline(owi, line);
  Adcirc::FileIO::Generic::sanitizeString(line);
  Adcirc::FileIO::Generic::splitString(line, split);
  int ilat = stoi(line.substr(5, 4));
  int ilon = stoi(line.substr(15, 4));
  std::string d1 = line.substr(68, 12);
  size_t nline = (ilat * ilon) / 8;
  if ((ilat * ilon) % 8 != 0) nline++;
  for (size_t i = 0; i < nline; ++i) {
    std::getline(owi, line);
  }
  std::getline(owi, line);
  std::string d2 = line.substr(68, 12);
  Date snap1 = dateFromOWIString(d1);
  Date snap2 = dateFromOWIString(d2);
  long long dt = snap2.toSeconds() - snap1.toSeconds();
  return std::make_tuple(dateStart, dateEnd, dt);
}

Date dateFromOWIString(const std::string &str) {
  int year, month, day, hour, minute, second;
  minute = 0;
  second = 0;
  year = stoi(str.substr(0, 4));
  month = stoi(str.substr(4, 2));
  day = stoi(str.substr(6, 2));
  hour = stoi(str.substr(8, 2));
  if (str.size() == 12) minute = stoi(str.substr(10, 2));
  return Date(year, month, day, hour, minute, second);
}

std::string toTidefacString(Date &d) {
  return boost::str(boost::format("%i,%i,%i,%i") % d.hour() % d.day() %
                    d.month() % d.year());
}

std::string toSwanString(Date &d) {
  return boost::str(boost::format("%04.4i%02.2i%02.2i.%02.2i%02.2i%02.2i") %
                    d.year() % d.month() % d.day() % d.hour() % d.minute() %
                    d.second());
}
