
#include "io.h"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/trim.hpp"
#include <boost/config/warning_disable.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/qi.hpp>
#include <complex>
#include <fstream>
#include <iostream>
#include <string>

IO::IO() {}

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

} // namespace parse

BOOST_FUSION_ADAPT_STRUCT(parse::node,
                          (int, id)(double, x)(double, y)(double, z))

BOOST_FUSION_ADAPT_STRUCT(parse::elem,
                          (int, id)(int, n)(int, n1)(int, n2)(int, n3))

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
} // namespace parse

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
