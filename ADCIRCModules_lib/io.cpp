//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#include "io.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "parsers.h"
#include <complex>
#include <fstream>
#include <iostream>
#include <string>

IO::IO() {}

int IO::readFileData(std::string filename, std::vector<std::string> &data) {
  std::ifstream t(filename.c_str());
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  std::string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);
  boost::algorithm::split(data, buffer, boost::is_any_of("\n"));
  t.close();
  return 0;
}

int IO::splitString(std::string &data, std::vector<std::string> &result) {
  boost::trim_if(data, boost::is_any_of(" "));
  boost::algorithm::split(result, data, boost::is_any_of(" "),
                          boost::token_compress_on);
  return 0;
}

int IO::splitStringNodeFormat(std::string &data, int &id, double &x, double &y,
                              double &z) {

  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef parse::node_parser<iterator_type> node_parser;

  node_parser np;
  parse::node n;
  std::string::const_iterator iter = data.begin();
  std::string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, np, space, n);
  if (r) {
    id = n.id;
    x = n.x;
    y = n.y;
    z = n.z;
    return 0;
  } else {
    return 1;
  }
}

int IO::splitStringElemFormat(std::string &data, int &id, int &n1, int &n2,
                              int &n3) {

  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef parse::elem_parser<iterator_type> elem_parser;

  elem_parser ep;
  parse::elem e;
  std::string::const_iterator iter = data.begin();
  std::string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, ep, space, e);
  if (r) {
    id = e.id;
    n1 = e.n1;
    n2 = e.n2;
    n3 = e.n3;
    return 0;
  } else {
    return 1;
  }
}

int IO::splitStringBoundary0Format(std::string &data, int &node1) {

  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef parse::boundary0_parser<iterator_type> boundary0_parser;

  boundary0_parser bp;
  parse::boundary0 b;
  std::string::const_iterator iter = data.begin();
  std::string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    return 0;
  } else {
    return 1;
  }
}

int IO::splitStringBoundary23Format(std::string &data, int &node1,
                                    double &crest, double &supercritical) {

  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef parse::boundary23_parser<iterator_type> boundary23_parser;

  boundary23_parser bp;
  parse::boundary23 b;
  std::string::const_iterator iter = data.begin();
  std::string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    crest = b.crest;
    supercritical = b.supercritical;
    return 0;
  } else {
    return 1;
  }
}

int IO::splitStringBoundary24Format(std::string &data, int &node1, int &node2,
                                    double &crest, double &subcritical,
                                    double &supercritical) {

  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef parse::boundary24_parser<iterator_type> boundary24_parser;

  boundary24_parser bp;
  parse::boundary24 b;
  std::string::const_iterator iter = data.begin();
  std::string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    node2 = b.node2;
    crest = b.crest;
    subcritical = b.subcritical;
    supercritical = b.supercritical;
    return 0;
  } else {
    return 1;
  }
}

int IO::splitStringBoundary25Format(std::string &data, int &node1, int &node2,
                                    double &crest, double &subcritical,
                                    double &supercritical, double &pipeheight,
                                    double &pipecoef, double &pipediam) {

  using boost::spirit::ascii::space;
  typedef std::string::const_iterator iterator_type;
  typedef parse::boundary25_parser<iterator_type> boundary25_parser;

  boundary25_parser bp;
  parse::boundary25 b;
  std::string::const_iterator iter = data.begin();
  std::string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    node2 = b.node2;
    crest = b.crest;
    subcritical = b.subcritical;
    supercritical = b.supercritical;
    pipeheight = b.pipeheight;
    pipecoef = b.pipecoef;
    pipediam = b.pipediam;
    return 0;
  } else {
    return 1;
  }
}
