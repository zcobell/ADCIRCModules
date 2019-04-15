/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
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
#include "logging.h"
#include <stdexcept>
#include <string>

static const std::string c_errorHeading("[ADCIRCModules ERROR]: ");
static const std::string c_warningHeading("[ADCIRCModules WARNING]: ");
static const std::string c_logHeading("[ADCIRCModules INFO]: ");

namespace Adcirc {

void Logging::throwError(const std::string &s) {
  throw std::runtime_error(c_errorHeading + s);
}

void Logging::throwError(const std::string &s, const char *file, int line) {
  throw std::runtime_error(c_errorHeading + s + " at " + file + ", line " +
                           std::to_string(line));
}

void Logging::warning(const std::string &s) {
  Logging::printMessage(c_warningHeading, s);
}

void Logging::log(const std::string &s) {
  Logging::printMessage(c_logHeading, s);
}

void Logging::printMessage(const std::string &header,
                           const std::string &message) {
  std::cout << header << message << std::endl;
}

}  // namespace Adcirc
