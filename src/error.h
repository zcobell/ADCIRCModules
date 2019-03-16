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
#ifndef ADCIRCMODULES_ERROR_H
#define ADCIRCMODULES_ERROR_H

#include <iostream>
#include <string>

namespace Adcirc {

enum _code { NoError, HasError };

/**
 * @class Error
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief The Error class is used to throw errors that can be caught in C++ or
 * python. The macro adcircmodules_throw_exception will describe the position in
 * the code that the error was thrown from
 */

class Error {
 public:
  Error() = default;

  static void throwError(const std::string &s);
  static void throwError(const std::string &s, const char *file, int line);

  static void warning(const std::string &s);
};

}  // namespace Adcirc

/**
 * @def adcircmodules_throw_exception
 * @brief Throws an exception to the user with the file and line number sources
 * from which the exception was thrown
 * @param arg string describing the error that is being thrown
 */
#define adcircmodules_throw_exception(arg) \
  Adcirc::Error::throwError(arg, __FILE__, __LINE__);

#endif  // ADCIRCMODULES_ERROR_H
