#ifndef PARSERS_H
#define PARSERS_H

#include "boost/config/warning_disable.hpp"
#include "boost/fusion/include/adapt_struct.hpp"
#include "boost/fusion/include/io.hpp"
#include "boost/spirit/include/phoenix_core.hpp"
#include "boost/spirit/include/phoenix_object.hpp"
#include "boost/spirit/include/phoenix_operator.hpp"
#include "boost/spirit/include/qi.hpp"

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

} // namespace parse

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

} // namespace parse

#endif // PARSERS_H
