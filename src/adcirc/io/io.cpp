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

//...Typedefs for boost parsers used in I/O
typedef string::const_iterator iterator_type;
typedef parse::node_parser<iterator_type> node_parser;
typedef parse::elem_parser<iterator_type> elem_parser;
typedef parse::boundary0_parser<iterator_type> boundary0_parser;
typedef parse::boundary23_parser<iterator_type> boundary23_parser;
typedef parse::boundary24_parser<iterator_type> boundary24_parser;
typedef parse::boundary25_parser<iterator_type> boundary25_parser;
typedef parse::nodalAttribute1_parser<iterator_type> nodalattribute1_parser;
typedef parse::nodalAttribute12_parser<iterator_type> nodalattribute12_parser;

IO::IO() {}

int IO::readFileData(string filename, vector<string> &data) {
  std::ifstream t(filename.c_str());
  t.seekg(0, std::ios::end);
  size_t size = t.tellg();
  string buffer(size, ' ');
  t.seekg(0);
  t.read(&buffer[0], size);
  boost::algorithm::split(data, buffer, boost::is_any_of("\n"));
  t.close();
  return FileIO::NoError;
}

int IO::splitString(string &data, vector<string> &result) {
  boost::trim_if(data, boost::is_any_of(" "));
  boost::algorithm::split(result, data, boost::is_any_of(" "),
                          boost::token_compress_on);
  return FileIO::NoError;
}

int IO::splitStringNodeFormat(string &data, int &id, double &x, double &y,
                              double &z) {
  node_parser np;
  parse::node n;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, np, space, n);
  if (r) {
    id = n.id;
    x = n.x;
    y = n.y;
    z = n.z;
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}

int IO::splitStringElemFormat(string &data, int &id, int &n1, int &n2,
                              int &n3) {
  elem_parser ep;
  parse::elem e;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, ep, space, e);
  if (r) {
    id = e.id;
    n1 = e.n1;
    n2 = e.n2;
    n3 = e.n3;
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}

int IO::splitStringBoundary0Format(string &data, int &node1) {

  boundary0_parser bp;
  parse::boundary0 b;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}

int IO::splitStringBoundary23Format(string &data, int &node1, double &crest,
                                    double &supercritical) {

  boundary23_parser bp;
  parse::boundary23 b;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    crest = b.crest;
    supercritical = b.supercritical;
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}

int IO::splitStringBoundary24Format(string &data, int &node1, int &node2,
                                    double &crest, double &subcritical,
                                    double &supercritical) {

  boundary24_parser bp;
  parse::boundary24 b;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    node2 = b.node2;
    crest = b.crest;
    subcritical = b.subcritical;
    supercritical = b.supercritical;
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}

int IO::splitStringBoundary25Format(string &data, int &node1, int &node2,
                                    double &crest, double &subcritical,
                                    double &supercritical, double &pipeheight,
                                    double &pipecoef, double &pipediam) {

  boundary25_parser bp;
  parse::boundary25 b;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

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
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}

int IO::splitStringAttribute1Format(string &data, int &node, double &value) {

  nodalattribute1_parser nap;
  parse::nodalAttribute1 na;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, nap, space, na);
  if (r) {
    node = na.node;
    value = na.value;
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}

int IO::splitStringAttribute12Format(string &data, int &node,
                                     vector<double> &values) {

  nodalattribute12_parser nap;
  parse::nodalAttribute12 na;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, nap, space, na);
  if (r) {
    node = na.node;
    values[0] = na.value1;
    values[1] = na.value2;
    values[2] = na.value3;
    values[3] = na.value4;
    values[4] = na.value5;
    values[5] = na.value6;
    values[6] = na.value7;
    values[7] = na.value8;
    values[8] = na.value9;
    values[9] = na.value10;
    values[10] = na.value11;
    values[11] = na.value12;
    return FileIO::NoError;
  } else {
    return FileIO::GenericFileReadError;
  }
}
