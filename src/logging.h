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
#ifndef ADCMOD_LOGGING_H
#define ADCMOD_LOGGING_H

#include <iostream>
#include <string>

namespace Adcirc {

/**
 * @class Logging
 * @author Zachary Cobell
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief The Logging class is used to throw errors and log messages to standard
 * output
 */
class Logging {
 public:
  Logging() = default;

  static void throwError(const std::string &s);
  static void throwError(const std::string &s, const char *file, int line);

  static void warning(const std::string &s);
  static void log(const std::string &s);

 private:
  static void printMessage(const std::string &header,
                           const std::string &message);
};

}  // namespace Adcirc

/**
 * @def adcircmodules_throw_exception
 * @brief Throws an exception to the user with the file and line number sources
 * from which the exception was thrown
 * @param arg string describing the error that is being thrown
 */
#define adcircmodules_throw_exception(arg) \
  Adcirc::Logging::throwError(arg, __FILE__, __LINE__)

#endif  // ADCMOD_LOGGING_H
