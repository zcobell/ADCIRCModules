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
#include "error.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "parsers.h"

using namespace std;

//...Typedefs for boost parsers used in I/O
using iterator_type = string::const_iterator;
using node_parser = parse::node_parser<iterator_type>;
using elem_parser = parse::elem_parser<iterator_type>;
using boundary0_parser = parse::boundary0_parser<iterator_type>;
using boundary23_parser = parse::boundary23_parser<iterator_type>;
using boundary24_parser = parse::boundary24_parser<iterator_type>;
using boundary25_parser = parse::boundary25_parser<iterator_type>;
using nodalattribute1_parser = parse::nodalAttribute1_parser<iterator_type>;
using nodalattribute2_parser = parse::nodalAttribute2_parser<iterator_type>;
using nodalattribute12_parser = parse::nodalAttribute12_parser<iterator_type>;
using harmonicsElevation_parser =
    parse::harmonicsElevation_parser<iterator_type>;
using harmonicsVelocity_parser = parse::harmonicsVelocity_parser<iterator_type>;

IO::IO() = default;

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

int IO::splitStringNodeFormat(string &data, size_t &id, double &x, double &y,
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
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringElemFormat(string &data, size_t &id, size_t &n1, size_t &n2,
                              size_t &n3) {
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
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringBoundary0Format(string &data, size_t &node1) {
  boundary0_parser bp;
  parse::boundary0 b;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, bp, space, b);
  if (r) {
    node1 = b.node1;
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringBoundary23Format(string &data, size_t &node1, double &crest,
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
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringBoundary24Format(string &data, size_t &node1, size_t &node2,
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
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringBoundary25Format(string &data, size_t &node1, size_t &node2,
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
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringAttribute1Format(string &data, size_t &node, double &value) {
  nodalattribute1_parser nap;
  parse::nodalAttribute1 na;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, nap, space, na);
  if (r) {
    node = na.node;
    value = na.value;
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringAttribute2Format(string &data, size_t &node, double &value1,
                                    double &value2) {
  nodalattribute2_parser nap;
  parse::nodalAttribute2 na;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();

  bool r = phrase_parse(iter, end, nap, space, na);
  if (r) {
    node = na.node;
    value1 = na.value1;
    value2 = na.value2;
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringAttribute12Format(string &data, size_t &node,
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
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringHarmonicsElevationFormat(string &data, double &amplitude,
                                            double &phase) {
  harmonicsElevation_parser hp;
  parse::harmonicsElevation harm;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();
  bool r = phrase_parse(iter, end, hp, space, harm);
  if (r) {
    amplitude = harm.amplitude;
    phase = harm.phase;
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}

int IO::splitStringHarmonicsVelocityFormat(string &data, double &u_magnitude,
                                           double &u_phase, double &v_magnitude,
                                           double &v_phase) {
  harmonicsVelocity_parser hp;
  parse::harmonicsVelocity harm;
  string::const_iterator iter = data.begin();
  string::const_iterator end = data.end();
  bool r = phrase_parse(iter, end, hp, space, harm);
  if (r) {
    u_magnitude = harm.u_magnitude;
    u_phase = harm.u_phase;
    v_magnitude = harm.v_magnitude;
    v_phase = harm.v_phase;
    return Adcirc::NoError;
  } else {
    adcircmodules_throw_exception("Error parsing file data");
    return Adcirc::NoError;
  }
}
