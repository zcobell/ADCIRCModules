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
#include "stringconversion.h"
#include "boost/algorithm/string.hpp"

using namespace std;

StringConversion::StringConversion() {}

double StringConversion::stringToDouble(string a, bool &ok) {
  ok = true;
  try {
    return std::stod(a);
  } catch (...) {
    ok = false;
    return 0.0;
  }
}

float StringConversion::stringToFloat(string a, bool &ok) {
  ok = true;
  try {
    return std::stof(a);
  } catch (...) {
    ok = false;
    return 0.0f;
  }
}

int StringConversion::stringToInt(string a, bool &ok) {
  ok = true;
  try {
    return std::stoi(a);
  } catch (...) {
    ok = false;
    return 0;
  }
}

size_t StringConversion::stringToSizet(string a, bool &ok) {
  ok = true;
  try {
    return std::stoull(a);
  } catch (...) {
    ok = false;
    return 0;
  }
}

string StringConversion::sanitizeString(string a) {
  string b = a;
  boost::algorithm::trim(b);
  b.erase(std::remove(b.begin(), b.end(), '\r'), b.end());
  return b;
}
