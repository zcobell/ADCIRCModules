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
#include "split.h"
#include "boost/algorithm/string/classification.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"

std::vector<std::string> Split::stringSplitToVector(std::string s,
                                                    const std::string &delim) {
  std::vector<std::string> elems;
  boost::trim_if(s, boost::is_any_of(delim));
  boost::algorithm::split(elems, s, boost::is_any_of(delim),
                          boost::token_compress_on);
  return elems;
}

std::vector<std::string> Split::stringSplitToVector(std::string s) {
  std::vector<std::string> elems;
  boost::trim_if(s, boost::is_any_of(" "));
  boost::algorithm::split(elems, s, boost::is_any_of(" "),
                          boost::token_compress_on);
  return elems;
}
