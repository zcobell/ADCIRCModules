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
#include "error.h"
#include <string>
#include <stdexcept>

using namespace Adcirc;
using namespace std;

static const string errorHeading("[ADCIRCModules runtime error]: ");

static const string warningHeading("[ADCIRCModules Warning]: ");

void Error::throwError(const string &s) {
  throw runtime_error(errorHeading + s);
}

void Error::throwError(const string &s, const char *file, int line) {
  throw runtime_error(errorHeading + s + " at " + file + ", line " +
                      to_string(line));
}

void Error::warning(const std::string &s) {
  std::cerr << warningHeading << s << std::endl;
}
