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
#ifndef ADCMOD_STRINGCONVERSION_H
#define ADCMOD_STRINGCONVERSION_H

#include <string>

class StringConversion {
 public:
  StringConversion() = default;

  static int stringToInt(const std::string& a, bool& ok);
  static size_t stringToSizet(const std::string& a, bool& ok);
  static float stringToFloat(const std::string& a, bool& ok);
  static double stringToDouble(const std::string& a, bool& ok);
  static std::string sanitizeString(const std::string& a);
};

#endif  // ADCMOD_STRINGCONVERSION_H
