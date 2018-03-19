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
#ifndef PARSERS_H
#define PARSERS_H

#include "boost/config/warning_disable.hpp"
#include "boost/fusion/include/adapt_struct.hpp"
#include "boost/fusion/include/io.hpp"
#include "boost/spirit/include/phoenix_core.hpp"
#include "boost/spirit/include/phoenix_object.hpp"
#include "boost/spirit/include/phoenix_operator.hpp"
#include "boost/spirit/include/qi.hpp"

using boost::spirit::ascii::space;

namespace parse {

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct node {
  int id;
  double x;
  double y;
  double z;
};

struct elem {
  int id;
  int n;
  int n1;
  int n2;
  int n3;
};

struct boundary0 {
  int node1;
};

struct boundary23 {
  int node1;
  double crest;
  double supercritical;
};

struct boundary24 {
  int node1;
  int node2;
  double crest;
  double supercritical;
  double subcritical;
};

struct boundary25 {
  int node1;
  int node2;
  double crest;
  double supercritical;
  double subcritical;
  double pipecoef;
  double pipeheight;
  double pipediam;
};

struct nodalAttribute1 {
  int node;
  double value;
};

struct nodalAttribute12 {
  int node;
  double value1;
  double value2;
  double value3;
  double value4;
  double value5;
  double value6;
  double value7;
  double value8;
  double value9;
  double value10;
  double value11;
  double value12;
};

}  // namespace parse

BOOST_FUSION_ADAPT_STRUCT(parse::node,
                          (int, id)(double, x)(double, y)(double, z))

BOOST_FUSION_ADAPT_STRUCT(parse::elem,
                          (int, id)(int, n)(int, n1)(int, n2)(int, n3))

BOOST_FUSION_ADAPT_STRUCT(parse::boundary0, (int, node1))

BOOST_FUSION_ADAPT_STRUCT(parse::boundary23,
                          (int, node1)(double, crest)(double, supercritical))

BOOST_FUSION_ADAPT_STRUCT(parse::boundary24,
                          (int, node1)(int, node2)(double, crest)(
                              double, subcritical)(double, supercritical))

BOOST_FUSION_ADAPT_STRUCT(
    parse::boundary25,
    (int, node1)(int, node2)(double, crest)(double, subcritical)(
        double, supercritical)(double, pipeheight)(double, pipecoef)(double,
                                                                     pipediam))

BOOST_FUSION_ADAPT_STRUCT(parse::nodalAttribute1, (int, node)(double, value))

BOOST_FUSION_ADAPT_STRUCT(
    parse::nodalAttribute12,
    (int, node)(double, value1)(double, value2)(double, value3)(double, value4)(
        double, value5)(double, value6)(double, value7)(double, value8)(
        double, value9)(double, value10)(double, value11)(double, value12))

namespace parse {
template <typename Iterator>
struct node_parser : qi::grammar<Iterator, node(), ascii::space_type> {
  node_parser() : node_parser::base_type(start) {
    using qi::double_;
    using qi::int_;
    start %= int_ >> double_ >> double_ >> double_;
  }
  qi::rule<Iterator, node(), ascii::space_type> start;
};

template <typename Iterator>
struct elem_parser : qi::grammar<Iterator, elem(), ascii::space_type> {
  elem_parser() : elem_parser::base_type(start) {
    using qi::double_;
    using qi::int_;
    start %= int_ >> int_ >> int_ >> int_ >> int_;
  }
  qi::rule<Iterator, elem(), ascii::space_type> start;
};

template <typename Iterator>
struct boundary0_parser
    : qi::grammar<Iterator, boundary0(), ascii::space_type> {
  boundary0_parser() : boundary0_parser::base_type(start) {
    using qi::int_;
    start %= int_;
  }
  qi::rule<Iterator, boundary0(), ascii::space_type> start;
};

template <typename Iterator>
struct boundary23_parser
    : qi::grammar<Iterator, boundary23(), ascii::space_type> {
  boundary23_parser() : boundary23_parser::base_type(start) {
    using qi::double_;
    using qi::int_;
    start %= int_ >> double_ >> double_;
  }
  qi::rule<Iterator, boundary23(), ascii::space_type> start;
};

template <typename Iterator>
struct boundary24_parser
    : qi::grammar<Iterator, boundary24(), ascii::space_type> {
  boundary24_parser() : boundary24_parser::base_type(start) {
    using qi::double_;
    using qi::int_;
    start %= int_ >> int_ >> double_ >> double_ >> double_;
  }
  qi::rule<Iterator, boundary24(), ascii::space_type> start;
};

template <typename Iterator>
struct boundary25_parser
    : qi::grammar<Iterator, boundary25(), ascii::space_type> {
  boundary25_parser() : boundary25_parser::base_type(start) {
    using qi::double_;
    using qi::int_;
    start %= int_ >> int_ >> double_ >> double_ >> double_ >> double_ >>
             double_ >> double_;
  }
  qi::rule<Iterator, boundary25(), ascii::space_type> start;
};

template <typename Iterator>
struct nodalAttribute1_parser
    : qi::grammar<Iterator, nodalAttribute1(), ascii::space_type> {
  nodalAttribute1_parser() : nodalAttribute1_parser::base_type(start) {
    using qi::double_;
    using qi::int_;
    start %= int_ >> double_;
  }
  qi::rule<Iterator, nodalAttribute1(), ascii::space_type> start;
};

template <typename Iterator>
struct nodalAttribute12_parser
    : qi::grammar<Iterator, nodalAttribute12(), ascii::space_type> {
  nodalAttribute12_parser() : nodalAttribute12_parser::base_type(start) {
    using qi::double_;
    using qi::int_;
    start %= int_ >> double_ >> double_ >> double_ >> double_ >> double_ >>
             double_ >> double_ >> double_ >> double_ >> double_ >> double_ >>
             double_;
  }
  qi::rule<Iterator, nodalAttribute12(), ascii::space_type> start;
};

}  // namespace parse

#endif  // PARSERS_H
