//------------------------------------------------------------------------//
// This file is part of Vortex.
//
// Vortex is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Vortex is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Vortex.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------//
#ifndef SPLIT_H
#define SPLIT_H

#include <sstream>
#include <string>
#include <vector>

class split {
 public:
  static std::vector<std::string> stringSplitToVector(std::string s,
                                                      char delim);
};

#endif  // SPLIT_H
