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
#include "fileio.h"
#include <complex>
#include <fstream>
#include <iostream>
#include <string>
#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/config/warning_disable.hpp"
#include "boost/spirit/include/phoenix.hpp"
#include "boost/spirit/include/qi.hpp"
#include "logging.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

/**
 * @brief Reads the data from a file and organizes it into a vector split by
 * line
 * @param[in] filename file to read
 * @param[out] data vector containing data split by newline characters
 */
void Adcirc::FileIO::Generic::readFileData(const std::string &filename,
                                           std::vector<std::string> &data) {
  std::ifstream t(filename.c_str());
  t.seekg(0, std::ios::end);
  std::streamoff size = t.tellg();
  std::string buffer(static_cast<size_t>(size), ' ');
  t.seekg(0);
  t.read(&buffer[0], size);
  boost::algorithm::split(data, buffer, boost::is_any_of("\n"));
  t.close();
  return;
}

/**
 * @brief Splits a string using spaces. Consecutive spaces are treated as one
 * delimiter
 * @param[in] data string to split
 * @param[out] fresult string split to vector
 */
void Adcirc::FileIO::Generic::splitString(std::string &data,
                                          std::vector<std::string> &fresult) {
  boost::trim_if(data, boost::is_any_of(" ,"));
  boost::algorithm::split(fresult, data, boost::is_any_of(" ,"),
                          boost::token_compress_on);
  return;
}

/**
 * @brief Returns the file extension by reverse searching for '.'
 * @param[in] filename filename to get extension for
 * @return file extension as a string
 */
std::string Adcirc::FileIO::Generic::getFileExtension(
    const std::string &filename) {
  size_t pos = filename.rfind('.');
  if (pos != std::string::npos) {
    return filename.substr(pos);
  }
  return "";
}

std::string Adcirc::FileIO::Generic::getFileWithoutExtension(
    const std::string &filename) {
  size_t pos = filename.rfind('.');
  if (pos != std::string::npos) {
    return std::string(filename.begin(), filename.begin() + pos);
  }
  return filename;
}

/**
 * @brief Checks if a file exists
 * @param[in] filename name of file to check for
 * @return true if file exists, otherwise false
 */
bool Adcirc::FileIO::Generic::fileExists(const std::string &filename) {
  std::ifstream ifile(filename.c_str());
  return static_cast<bool>(ifile);
}

std::string Adcirc::FileIO::Generic::sanitizeString(const std::string &a) {
  std::string b = a;
  boost::algorithm::trim(b);
  boost::algorithm::replace_all(b, "\t", " ");
  b.erase(std::remove(b.begin(), b.end(), '\r'), b.end());
  return b;
}

/**
 * @brief Splits a string from an ADCIRC mesh file into the data required to
 * generate an adcirc node object
 * @param[in] data string read from file
 * @param[out] id node id
 * @param[out] x node x-position
 * @param[out] y node y-position
 * @param[out] z node z-position
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringNodeFormat(const std::string &data,
                                                     size_t &id, double &x,
                                                     double &y, double &z) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(id) = qi::_1] >>
                           qi::double_[phoenix::ref(x) = qi::_1] >>
                           qi::double_[phoenix::ref(y) = qi::_1] >>
                           qi::double_[phoenix::ref(z) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits a string from an ADCIRC mesh file to the data for an element
 * @param[in] data string read from file
 * @param[out] id element id
 * @param[out] nodes vector of nodes found in the file
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringElemFormat(
    const std::string &data, size_t &id, std::vector<size_t> &nodes) {
  return qi::phrase_parse(
      data.begin(), data.end(),
      (qi::int_[phoenix::ref(id) = qi::_1] >> qi::int_ >>
       *(qi::int_[phoenix::push_back(phoenix::ref(nodes), qi::_1)])),
      ascii::space);
}

/**
 * @brief Splits a string for a single node boundary
 * @param[in] data string read from file
 * @param[out] node1 node for boundary data
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringBoundary0Format(
    const std::string &data, size_t &node1) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits a string for an external weir boundary
 * @param[in] data string read from file
 * @param[out] node1 node for boundary data
 * @param[out] crest crest elevation for boundary
 * @param[out] supercritical supercritical weir coefficient
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringBoundary23Format(
    const std::string &data, size_t &node1, double &crest,
    double &supercritical) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1] >>
                           qi::double_[phoenix::ref(crest) = qi::_1] >>
                           qi::double_[phoenix::ref(supercritical) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits a string for an internal weir boundary
 * @param[in] data string read from file
 * @param[out] node1 front side node for weir
 * @param[out] node2 back side node for weir
 * @param[out] crest crest elevation for weir
 * @param[out] subcritical subcritical weir coefficient
 * @param[out] supercritical supercritical weir coefficient
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringBoundary24Format(
    const std::string &data, size_t &node1, size_t &node2, double &crest,
    double &subcritical, double &supercritical) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1] >>
                           qi::int_[phoenix::ref(node2) = qi::_1] >>
                           qi::double_[phoenix::ref(crest) = qi::_1] >>
                           qi::double_[phoenix::ref(subcritical) = qi::_1] >>
                           qi::double_[phoenix::ref(supercritical) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits a string for an internal weir with pipes
 * @param[in] data string read from file
 * @param[out] node1 front side node for weir
 * @param[out] node2 back side node for weir
 * @param[out] crest crest elevation for weir
 * @param[out] subcritical subcritical weir coefficient
 * @param[out] supercritical supercritical weir coefficient
 * @param[out] pipeheight height of pipe relative to datum
 * @param[out] pipecoef coefficient of pipe flow
 * @param[out] pipediam diameter of pipe
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringBoundary25Format(
    const std::string &data, size_t &node1, size_t &node2, double &crest,
    double &subcritical, double &supercritical, double &pipeheight,
    double &pipecoef, double &pipediam) {
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

/**
 * @brief Splits a string in format of nodeid, value1
 * @param[in] data string read from file
 * @param[out] node node id for data
 * @param[out] value data specified for the node
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringAttribute1Format(
    const std::string &data, size_t &node, double &value) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node) = qi::_1] >>
                           qi::double_[phoenix::ref(value) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits a string in the format of nodeid, value1, value2
 * @param[in] data string read from file
 * @param[out] node node id for data
 * @param[out] value1 first value specified for the node
 * @param[out] value2 second value specified for the node
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringAttribute2Format(
    const std::string &data, size_t &node, double &value1, double &value2) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node) = qi::_1] >>
                           qi::double_[phoenix::ref(value1) = qi::_1] >>
                           qi::double_[phoenix::ref(value2) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits a string in the format node, value_1 , ... , value_n
 * @param[in] data string read from file
 * @param[out] node node id for data
 * @param[out] values vector of n values
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringAttributeNFormat(
    const std::string &data, size_t &node, std::vector<double> &values) {
  return qi::phrase_parse(
      data.begin(), data.end(),
      (qi::int_[phoenix::ref(node) = qi::_1] >>
       *(qi::double_[phoenix::push_back(phoenix::ref(values), qi::_1)])),
      ascii::space);
}

/**
 * @brief Splits the harmonics elevation data read from an adcirc ascii
 * harmonics file
 * @param[in] data string read from file
 * @param[out] amplitude amplitude specified at the node constituent
 * @param[out] phase phase spcified at the node for this constituent
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringHarmonicsElevationFormat(
    const std::string &data, double &amplitude, double &phase) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::double_[phoenix::ref(amplitude) = qi::_1] >>
                           qi::double_[phoenix::ref(phase) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits the harmonics velocity data read from an adcirc ascii
 * harmonics file
 * @param[in] data string read from file
 * @param[out] u_magnitude u-component of velocity magnitude
 * @param[out] u_phase u-component of velocity phase
 * @param[out] v_magnitude v-component of velocity magnitude
 * @param[out] v_phase v-component of velocity phase
 * @return true if successful read
 */
bool Adcirc::FileIO::AdcircIO::splitStringHarmonicsVelocityFormat(
    const std::string &data, double &u_magnitude, double &u_phase,
    double &v_magnitude, double &v_phase) {
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::double_[phoenix::ref(u_magnitude) = qi::_1] >>
                           qi::double_[phoenix::ref(u_phase) = qi::_1] >>
                           qi::double_[phoenix::ref(v_magnitude) = qi::_1] >>
                           qi::double_[phoenix::ref(v_phase) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits the data in an SMS format 2dm node string
 * @param[in] data string read from file
 * @param[out] id node id for this line
 * @param[out] x node x-position
 * @param[out] y node y-position
 * @param[out] z node z-position
 * @return true if successful read
 */
bool Adcirc::FileIO::SMSIO::splitString2dmNodeFormat(const std::string &data,
                                                     size_t &id, double &x,
                                                     double &y, double &z) {
  return qi::phrase_parse(data.begin() + 3, data.end(),
                          (qi::int_[phoenix::ref(id) = qi::_1] >>
                           qi::double_[phoenix::ref(x) = qi::_1] >>
                           qi::double_[phoenix::ref(y) = qi::_1] >>
                           qi::double_[phoenix::ref(z) = qi::_1]),
                          ascii::space);
}

/**
 * @brief Splits the data in an SMS format 2dm element string
 * @param[in] data string read from file
 * @param[out] id node id read
 * @param[out] nodes list of nodes in the element
 * @return true if successful read
 */
bool Adcirc::FileIO::SMSIO::splitString2dmElementFormat(
    const std::string &data, size_t &id, std::vector<size_t> &nodes) {
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

bool Adcirc::FileIO::HMDFIO::splitStringHmdfFormat(const std::string &data,
                                                   int &year, int &month,
                                                   int &day, int &hour,
                                                   int &minute, int &second,
                                                   double &value) {
  bool r = qi::phrase_parse(data.begin(), data.end(),
                            qi::int_[phoenix::ref(year) = qi::_1] >>
                                qi::int_[phoenix::ref(month) = qi::_1] >>
                                qi::int_[phoenix::ref(day) = qi::_1] >>
                                qi::int_[phoenix::ref(hour) = qi::_1] >>
                                qi::int_[phoenix::ref(minute) = qi::_1] >>
                                qi::int_[phoenix::ref(second) = qi::_1] >>
                                qi::double_[phoenix::ref(value) = qi::_1],
                            qi::space);
  if (!r) {
    r = qi::phrase_parse(data.begin(), data.end(),
                         qi::int_[phoenix::ref(year) = qi::_1] >>
                             qi::int_[phoenix::ref(month) = qi::_1] >>
                             qi::int_[phoenix::ref(day) = qi::_1] >>
                             qi::int_[phoenix::ref(hour) = qi::_1] >>
                             qi::int_[phoenix::ref(minute) = qi::_1] >>
                             qi::double_[phoenix::ref(value) = qi::_1],
                         qi::space);
    second = 0;
  }

  return r;
}
