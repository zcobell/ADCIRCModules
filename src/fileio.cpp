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
#include "fileio.h"
#include <complex>
#include <fstream>
#include <iostream>
#include <string>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/config/warning_disable.hpp"
#include "boost/spirit/include/phoenix_core.hpp"
#include "boost/spirit/include/phoenix_operator.hpp"
#include "boost/spirit/include/phoenix_stl.hpp"
#include "boost/spirit/include/qi.hpp"
#include "error.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

namespace FileIO {

namespace Generic {

void readFileData(const std::string &filename, std::vector<std::string> &data) {
  std::ifstream t(filename.c_str());
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  std::string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);
  boost::algorithm::split(data, buffer, boost::is_any_of("\n"));
  t.close();
  return;
}

void splitString(std::string &data, std::vector<std::string> &fresult) {
  boost::trim_if(data, boost::is_any_of(" "));
  boost::algorithm::split(fresult, data, boost::is_any_of(" "),
                          boost::token_compress_on);
  return;
}

std::string getFileExtension(const std::string &filename) {
  size_t pos = filename.rfind('.');
  if (pos != std::string::npos) {
    return filename.substr(pos);
  }
  return "";
}

bool fileExists(const std::string &filename) {
  std::ifstream ifile(filename.c_str());
  return (bool)ifile;
}
}  // namespace Generic

namespace AdcircIO {
bool splitStringNodeFormat(std::string &data, size_t &id, double &x, double &y,
                           double &z) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(id) = qi::_1] >>
                           qi::double_[phoenix::ref(x) = qi::_1] >>
                           qi::double_[phoenix::ref(y) = qi::_1] >>
                           qi::double_[phoenix::ref(z) = qi::_1]),
                          ascii::space);
}

bool splitStringElemFormat(std::string &data, size_t &id,
                           std::vector<size_t> &nodes) {
  return qi::phrase_parse(
      data.begin(), data.end(),
      (qi::int_[phoenix::ref(id) = qi::_1] >> qi::int_ >>
       *(qi::int_[phoenix::push_back(phoenix::ref(nodes), qi::_1)])),
      ascii::space);
}

bool splitStringBoundary0Format(std::string &data, size_t &node1) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1]),
                          ascii::space);
}

bool splitStringBoundary23Format(std::string &data, size_t &node1,
                                 double &crest, double &supercritical) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1] >>
                           qi::double_[phoenix::ref(crest) = qi::_1] >>
                           qi::double_[phoenix::ref(supercritical) = qi::_1]),
                          ascii::space);
}

bool splitStringBoundary24Format(std::string &data, size_t &node1,
                                 size_t &node2, double &crest,
                                 double &subcritical, double &supercritical) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1] >>
                           qi::int_[phoenix::ref(node2) = qi::_1] >>
                           qi::double_[phoenix::ref(crest) = qi::_1] >>
                           qi::double_[phoenix::ref(subcritical) = qi::_1] >>
                           qi::double_[phoenix::ref(supercritical) = qi::_1]),
                          ascii::space);
}

bool splitStringBoundary25Format(std::string &data, size_t &node1,
                                 size_t &node2, double &crest,
                                 double &subcritical, double &supercritical,
                                 double &pipeheight, double &pipecoef,
                                 double &pipediam) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1] >>
                           qi::int_[phoenix::ref(node2) = qi::_1] >>
                           qi::double_[phoenix::ref(crest) = qi::_1] >>
                           qi::double_[phoenix::ref(subcritical) = qi::_1] >>
                           qi::double_[phoenix::ref(supercritical) = qi::_1] >>
                           qi::double_[phoenix::ref(pipeheight) = qi::_1] >>
                           qi::double_[phoenix::ref(pipecoef) = qi::_1] >>
                           qi::double_[phoenix::ref(pipediam) = qi::_1]),
                          ascii::space);
}

bool splitStringAttribute1Format(std::string &data, size_t &node,
                                 double &value) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node) = qi::_1] >>
                           qi::double_[phoenix::ref(value) = qi::_1]),
                          ascii::space);
}

bool splitStringAttribute2Format(std::string &data, size_t &node,
                                 double &value1, double &value2) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node) = qi::_1] >>
                           qi::double_[phoenix::ref(value1) = qi::_1] >>
                           qi::double_[phoenix::ref(value2) = qi::_1]),
                          ascii::space);
}

bool splitStringAttributeNFormat(std::string &data, size_t &node,
                                 std::vector<double> &values) {
  return qi::phrase_parse(
      data.begin(), data.end(),
      (qi::int_[phoenix::ref(node) = qi::_1] >>
       *(qi::double_[phoenix::push_back(phoenix::ref(values), qi::_1)])),
      ascii::space);
}

bool splitStringHarmonicsElevationFormat(std::string &data, double &amplitude,
                                         double &phase) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::double_[phoenix::ref(amplitude) = qi::_1] >>
                           qi::double_[phoenix::ref(phase) = qi::_1]),
                          ascii::space);
}

bool splitStringHarmonicsVelocityFormat(std::string &data, double &u_magnitude,
                                        double &u_phase, double &v_magnitude,
                                        double &v_phase) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::double_[phoenix::ref(u_magnitude) = qi::_1] >>
                           qi::double_[phoenix::ref(u_phase) = qi::_1] >>
                           qi::double_[phoenix::ref(v_magnitude) = qi::_1] >>
                           qi::double_[phoenix::ref(v_phase) = qi::_1]),
                          ascii::space);
}
}  // namespace Adcirc
namespace SMSIO {
bool splitString2dmNodeFormat(std::string &data, size_t &id, double &x,
                              double &y, double &z) {
  return qi::phrase_parse(data.begin() + 3, data.end(),
                          (qi::int_[phoenix::ref(id) = qi::_1] >>
                           qi::double_[phoenix::ref(x) = qi::_1] >>
                           qi::double_[phoenix::ref(y) = qi::_1] >>
                           qi::double_[phoenix::ref(z) = qi::_1]),
                          ascii::space);
}

bool splitString2dmElementFormat(std::string &data, size_t &id,
                                 std::vector<size_t> &nodes) {
  std::string key = data.substr(0, 3);
  if (key == "E3T") {
    nodes.resize(3);
    return qi::phrase_parse(data.begin() + 3, data.end(),
                            (qi::int_[phoenix::ref(id) = qi::_1] >>
                             qi::int_[phoenix::ref(nodes[0]) = qi::_1] >>
                             qi::int_[phoenix::ref(nodes[1]) = qi::_1] >>
                             qi::int_[phoenix::ref(nodes[2]) = qi::_1]),
                            qi::space);
  } else if (key == "E4Q") {
    nodes.resize(4);
    return qi::phrase_parse(data.begin() + 3, data.end(),
                            (qi::int_[phoenix::ref(id) = qi::_1] >>
                             qi::int_[phoenix::ref(nodes[0]) = qi::_1] >>
                             qi::int_[phoenix::ref(nodes[1]) = qi::_1] >>
                             qi::int_[phoenix::ref(nodes[2]) = qi::_1] >>
                             qi::int_[phoenix::ref(nodes[3]) = qi::_1]),
                            qi::space);
  }
  return false;
}
}  // namespace SMS
}  // namespace FileIO
