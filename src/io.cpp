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
#include "io.h"
#include <complex>
#include <fstream>
#include <iostream>
#include <string>
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "error.h"

#if USE_BOOSTSPIRITX3
#include "boost/spirit/home/x3.hpp"
using namespace std;
using boost::spirit::x3::_attr;
using boost::spirit::x3::double_;
using boost::spirit::x3::int_;
using boost::spirit::x3::phrase_parse;
using boost::spirit::x3::ascii::space;
#else
#include "boost/config/warning_disable.hpp"
#include "boost/spirit/include/phoenix_core.hpp"
#include "boost/spirit/include/phoenix_operator.hpp"
#include "boost/spirit/include/phoenix_stl.hpp"
#include "boost/spirit/include/qi.hpp"
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;
#endif

using namespace std;

int IO::readFileData(const string &filename, vector<string> &data) {
  std::ifstream t(filename.c_str());
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);
  boost::algorithm::split(data, buffer, boost::is_any_of("\n"));
  t.close();
  return Adcirc::NoError;
}

int IO::splitString(string &data, vector<string> &fresult) {
  boost::trim_if(data, boost::is_any_of(" "));
  boost::algorithm::split(fresult, data, boost::is_any_of(" "),
                          boost::token_compress_on);
  return Adcirc::NoError;
}

bool IO::splitStringNodeFormat(string &data, size_t &id, double &x, double &y,
                               double &z) {
#if USE_BOOSTSPIRITX3
  auto assign_id = [&](auto &ctx) { id = _attr(ctx); };
  auto assign_x = [&](auto &ctx) { x = _attr(ctx); };
  auto assign_y = [&](auto &ctx) { y = _attr(ctx); };
  auto assign_z = [&](auto &ctx) { z = _attr(ctx); };
  return phrase_parse(data.begin(), data.end(),
                      (int_[assign_id] >> double_[assign_x] >>
                       double_[assign_y] >> double_[assign_z]),
                      space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(id) = qi::_1] >>
                           qi::double_[phoenix::ref(x) = qi::_1] >>
                           qi::double_[phoenix::ref(y) = qi::_1] >>
                           qi::double_[phoenix::ref(z) = qi::_1]),
                          ascii::space);
#endif
}

bool IO::splitStringElemFormat(string &data, size_t &id,
                               vector<size_t> &nodes) {
#if USE_BOOSTSPIRITX3
  auto assign_id = [&](auto &ctx) { id = _attr(ctx); };
  auto push_back = [&](auto &ctx) { nodes.push_back(_attr(ctx)); };
  return phrase_parse(data.begin(), data.end(),
                      (int_[assign_id] >> int_ >> *(int_[push_back])),
                      boost::spirit::x3::omit[+space]);
#else
  return qi::phrase_parse(
      data.begin(), data.end(),
      (qi::int_[phoenix::ref(id)] >> qi::int_ >>
       *(qi::int_[phoenix::push_back(phoenix::ref(nodes), qi::_1)])),
      ascii::space);
#endif
}

bool IO::splitStringBoundary0Format(string &data, size_t &node1) {
#if USE_BOOSTSPIRITX3
  auto assign_id = [&](auto &ctx) { node1 = _attr(ctx); };
  return phrase_parse(data.begin(), data.end(), (int_[assign_id]), space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1]),
                          ascii::space);
#endif
}

bool IO::splitStringBoundary23Format(string &data, size_t &node1, double &crest,
                                     double &supercritical) {
#if USE_BOOSTSPIRITX3
  auto assign_id = [&](auto &ctx) { node1 = _attr(ctx); };
  auto assign_crest = [&](auto &ctx) { crest = _attr(ctx); };
  auto assign_supercritical = [&](auto &ctx) { supercritical = _attr(ctx); };
  return phrase_parse(data.begin(), data.end(),
                      (int_[assign_id] >> double_[assign_crest] >>
                       double_[assign_supercritical]),
                      space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1] >>
                           qi::double_[phoenix::ref(crest) = qi::_1] >>
                           qi::double_[phoenix::ref(supercritical) = qi::_1]),
                          ascii::space);
#endif
}

bool IO::splitStringBoundary24Format(string &data, size_t &node1, size_t &node2,
                                     double &crest, double &subcritical,
                                     double &supercritical) {
#if USE_BOOSTSPIRITX3
  auto assign_n1 = [&](auto &ctx) { node1 = _attr(ctx); };
  auto assign_n2 = [&](auto &ctx) { node2 = _attr(ctx); };
  auto assign_crest = [&](auto &ctx) { crest = _attr(ctx); };
  auto assign_subcritical = [&](auto &ctx) { subcritical = _attr(ctx); };
  auto assign_supercritical = [&](auto &ctx) { supercritical = _attr(ctx); };
  return phrase_parse(
      data.begin(), data.end(),
      (int_[assign_n1] >> int_[assign_n2] >> double_[assign_crest] >>
       double_[assign_subcritical] >> double_[assign_supercritical]),
      space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node1) = qi::_1] >>
                           qi::int_[phoenix::ref(node2) = qi::_1] >>
                           qi::double_[phoenix::ref(crest) = qi::_1] >>
                           qi::double_[phoenix::ref(subcritical) = qi::_1] >>
                           qi::double_[phoenix::ref(supercritical) = qi::_1]),
                          ascii::space);
#endif
}

bool IO::splitStringBoundary25Format(string &data, size_t &node1, size_t &node2,
                                     double &crest, double &subcritical,
                                     double &supercritical, double &pipeheight,
                                     double &pipecoef, double &pipediam) {
#if USE_BOOSTSPIRITX3
  auto assign_n1 = [&](auto &ctx) { node1 = _attr(ctx); };
  auto assign_n2 = [&](auto &ctx) { node2 = _attr(ctx); };
  auto assign_crest = [&](auto &ctx) { crest = _attr(ctx); };
  auto assign_subcritical = [&](auto &ctx) { subcritical = _attr(ctx); };
  auto assign_supercritical = [&](auto &ctx) { supercritical = _attr(ctx); };
  auto assign_pipeheight = [&](auto &ctx) { pipeheight = _attr(ctx); };
  auto assign_pipecoef = [&](auto &ctx) { pipecoef = _attr(ctx); };
  auto assign_pipediam = [&](auto &ctx) { pipediam = _attr(ctx); };
  return phrase_parse(
      data.begin(), data.end(),
      (int_[assign_n1] >> int_[assign_n2] >> double_[assign_crest] >>
       double_[assign_subcritical] >> double_[assign_supercritical] >>
       double_[assign_pipeheight] >> double_[assign_pipecoef] >>
       double_[assign_pipediam]),
      space);
#else
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
#endif
}

bool IO::splitStringAttribute1Format(string &data, size_t &node,
                                     double &value) {
#if USE_BOOSTSPIRITX3
  auto assign_id = [&](auto &ctx) { node = _attr(ctx); };
  auto assign_value = [&](auto &ctx) { value = _attr(ctx); };
  return phrase_parse(data.begin(), data.end(),
                      (int_[assign_id] >> double_[assign_value]), space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node) = qi::_1] >>
                           qi::double_[phoenix::ref(value) = qi::_1]),
                          ascii::space);
#endif
}

bool IO::splitStringAttribute2Format(string &data, size_t &node, double &value1,
                                     double &value2) {
#if USE_BOOSTSPIRITX3
  auto assign_id = [&](auto &ctx) { node = _attr(ctx); };
  auto assign_value1 = [&](auto &ctx) { value1 = _attr(ctx); };
  auto assign_value2 = [&](auto &ctx) { value2 = _attr(ctx); };
  return phrase_parse(
      data.begin(), data.end(),
      (int_[assign_id] >> double_[assign_value1] >> double_[assign_value2]),
      space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::int_[phoenix::ref(node) = qi::_1] >>
                           qi::double_[phoenix::ref(value1) = qi::_1] >>
                           qi::double_[phoenix::ref(value2) = qi::_1]),
                          ascii::space);
#endif
}

bool IO::splitStringAttributeNFormat(string &data, size_t &node,
                                     vector<double> &values) {
#if USE_BOOSTSPIRITX3
  auto assign_id = [&](auto &ctx) { node = _attr(ctx); };
  auto push_back = [&](auto &ctx) { values.push_back(_attr(ctx)); };
  return phrase_parse(data.begin(), data.end(),
                      (int_[assign_id] >> *(double_[push_back])),
                      boost::spirit::x3::omit[+space]);
#else
  return qi::phrase_parse(
      data.begin(), data.end(),
      (qi::int_[phoenix::ref(node) = qi::_1] >>
       *(qi::double_[phoenix::push_back(phoenix::ref(values), qi::_1)])),
      ascii::space);
#endif
}

bool IO::splitStringHarmonicsElevationFormat(string &data, double &amplitude,
                                             double &phase) {
#if USE_BOOSTSPIRITX3
  auto assign_amp = [&](auto &ctx) { amplitude = _attr(ctx); };
  auto assign_pha = [&](auto &ctx) { phase = _attr(ctx); };
  return phrase_parse(data.begin(), data.end(),
                      (double_[assign_amp] >> double_[assign_pha]), space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::double_[phoenix::ref(amplitude) = qi::_1] >>
                           qi::double_[phoenix::ref(phase) = qi::_1]),
                          ascii::space);
#endif
}

bool IO::splitStringHarmonicsVelocityFormat(string &data, double &u_magnitude,
                                            double &u_phase,
                                            double &v_magnitude,
                                            double &v_phase) {
#if USE_BOOSTSPIRITX3
  auto assign_umag = [&](auto &ctx) { u_magnitude = _attr(ctx); };
  auto assign_upha = [&](auto &ctx) { u_phase = _attr(ctx); };
  auto assign_vmag = [&](auto &ctx) { v_magnitude = _attr(ctx); };
  auto assign_vpha = [&](auto &ctx) { v_phase = _attr(ctx); };
  return phrase_parse(data.begin(), data.end(),
                      (double_[assign_umag] >> double_[assign_upha] >>
                       double_[assign_vmag] >> double_[assign_vpha]),
                      space);
#else
  return qi::phrase_parse(data.begin(), data.end(),
                          (qi::double_[phoenix::ref(u_magnitude) = qi::_1] >>
                           qi::double_[phoenix::ref(u_phase) = qi::_1] >>
                           qi::double_[phoenix::ref(v_magnitude) = qi::_1] >>
                           qi::double_[phoenix::ref(v_phase) = qi::_1]),
                          ascii::space);
#endif
}
