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
#ifndef ERROR_H
#define ERROR_H

#include <iostream>
#include <string>
#include "adcircmodules_global.h"

namespace Adcirc {

enum _code { NoError, HasError };

class Error {
 public:
  Error() = default;

  static void throwError(const std::string &s);
  static void throwError(const std::string &s, const char *file, int line);

  static void warning(const std::string &s);
};

}  // namespace Adcirc

#define adcircmodules_throw_exception(arg) Adcirc::Error::throwError(arg, __FILE__, __LINE__);

#endif  // ERROR_H
