#include "oceanweatherrecord.h"
#include <cassert>
#include "boost/config/warning_disable.hpp"
#include "boost/format.hpp"
#include "boost/spirit/include/phoenix.hpp"
#include "boost/spirit/include/qi.hpp"
#include "logging.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

OceanweatherRecord::OceanweatherRecord()
    : m_backgroundPressure(1013.0), m_gridchanged(true) {}

void OceanweatherRecord::addDomain(std::fstream *fid_pressure,
                                   std::fstream *fid_wind) {
  Domain d;
  d.fid_pressure = fid_pressure;
  d.fid_wind = fid_wind;
  this->m_domains.push_back(d);
}

int OceanweatherRecord::read() {
  bool haserror = false;
  for (auto &d : m_domains) {
    if (d.fid_pressure->peek() && d.fid_wind->peek()) {
      std::string header_pressure, header_wind;
      std::getline(*(d.fid_pressure), header_pressure);
      std::getline(*(d.fid_wind), header_wind);

      Grid p = parseOwiGridLine(header_pressure);
      Grid w = parseOwiGridLine(header_wind);
      if (p != w) {
        oceanweather_throw_exception(
            "Grids between pressure and wind fields are not identical");
      }

      if (p != d.grid) {
        m_gridchanged = true;
        d.grid = std::move(p);
      } else {
        m_gridchanged = false;
      }

      int ierr = this->readData(d, d.fid_pressure, d.data_pressure);
      ierr = this->readData(d, d.fid_wind, d.data_u);
      ierr = this->readData(d, d.fid_wind, d.data_v);
    } else {
      Logging::warning("Reached end of file");
      haserror = true;
    }
  }
  return haserror;
}

size_t OceanweatherRecord::ndomain() const { return this->m_domains.size(); }

OceanweatherRecord::Domain *OceanweatherRecord::domain(size_t index) {
  assert(index < ndomains());
  return &m_domains[index];
}

std::tuple<double, double, double, size_t> OceanweatherRecord::get(
    const size_t i, const size_t j, const size_t domain) const {
  const size_t idx = m_domains[domain].grid.nx * j + i;
  return idx < m_domains[domain].data_pressure.size()
             ? std::make_tuple(m_domains[domain].data_pressure[idx],
                               m_domains[domain].data_u[idx],
                               m_domains[domain].data_v[idx], domain)
             : std::make_tuple(backgroundPressure(), 0.0, 0.0,
                               std::numeric_limits<size_t>::max());
}

std::tuple<double, double, double, size_t> OceanweatherRecord::get(
    const double x, const double y, size_t domain) const {
  if (domain == std::numeric_limits<size_t>::max()) {
    domain = findDomain(x, y);
    if (domain == std::numeric_limits<size_t>::max()) {
      return {backgroundPressure(), 0.0, 0.0, domain};
    }
  }

  size_t i, j;
  double w1, w2, w3, w4;
  std::tie(i, j, domain, w1, w2, w3, w4) = interpolationWeight(x, y, domain);

  double p1, u1, v1;
  std::tie(p1, u1, v1, std::ignore) = this->get(i, j, domain);

  double p2, u2, v2;
  std::tie(p2, u2, v2, std::ignore) = this->get(i + 1, j, domain);

  double p3, u3, v3;
  std::tie(p3, u3, v3, std::ignore) = this->get(i + 1, j + 1, domain);

  double p4, u4, v4;
  std::tie(p4, u4, v4, std::ignore) = this->get(i + 1, j, domain);

  double pv = w1 * p1 + w2 * p2 + w3 * p3 + w4 * p4;
  double uv = w1 * u1 + w2 * u2 + w3 * u3 + w4 * u4;
  double vv = w1 * v1 + w2 * v2 + w3 * v3 + w4 * v4;
  return {pv, uv, vv, domain};
}

std::tuple<size_t, size_t, size_t, double, double, double, double>
OceanweatherRecord::interpolationWeight(const double x, const double y,
                                        size_t domain) const {
  if (domain == std::numeric_limits<size_t>::max()) {
    domain = this->findDomain(x, y);
    if (domain == std::numeric_limits<size_t>::max()) {
      return {0.0, 0.0, domain, 0.0, 0.0, 0.0, 0.0};
    }
  }
  if (inDomain(domain, x, y)) {
    int xi = std::floor((x - m_domains[domain].grid.xmin) /
                        m_domains[domain].grid.dx);
    int yj = std::floor((y - m_domains[domain].grid.ymin) /
                        m_domains[domain].grid.dy);
    double x1 = m_domains[domain].grid.x(xi);
    double x2 = m_domains[domain].grid.x(xi + 1);
    double y1 = m_domains[domain].grid.y(yj);
    double y2 = m_domains[domain].grid.y(yj + 1);
    double w = (x2 - x1) * (y2 - y1);
    double w1 = ((x2 - x) * (y2 - y)) / w;
    double w2 = ((x - x1) * (y2 - y)) / w;
    double w3 = ((x - x1) * (y - y1)) / w;
    double w4 = ((x2 - x) * (y - y1)) / w;
    return {xi, yj, domain, w1, w2, w3, w4};
  } else {
    return {0.0, 0.0, domain, 0.0, 0.0, 0.0, 0.0};
  }
}

bool OceanweatherRecord::inDomain(size_t domain, const double x,
                                  const double y) const {
  return x >= m_domains[domain].grid.xmin && x < m_domains[domain].grid.xmax &&
         y >= m_domains[domain].grid.ymin && y < m_domains[domain].grid.ymax;
}

size_t OceanweatherRecord::findDomain(const double x, const double y) const {
  for (int i = ndomain() - 1; i >= 0; --i) {
    if (this->inDomain(i, x, y)) {
      return i;
    }
  }
  return std::numeric_limits<size_t>::max();
}

bool OceanweatherRecord::gridChanged() const { return m_gridchanged; }

double OceanweatherRecord::backgroundPressure() const {
  return m_backgroundPressure;
}

void OceanweatherRecord::setBackgroundPressure(double backgroundPressure) {
  m_backgroundPressure = backgroundPressure;
}

int OceanweatherRecord::readData(const OceanweatherRecord::Domain &d,
                                 std::fstream *fid,
                                 std::vector<double> &array) {
  constexpr size_t rpl = 8;  // 8 records per line for OWI format
  const size_t nv = d.grid.nx * d.grid.ny;

  array.clear();
  array.reserve(nv);

  for (size_t i = 0; i < nv; i += rpl) {
    std::string line;
    std::getline(*(fid), line);
    qi::phrase_parse(
        line.begin(), line.end(),
        (*(qi::double_[phoenix::push_back(phoenix::ref(array), qi::_1)])),
        ascii::space);
  }
  return 0;
}

OceanweatherRecord::Grid OceanweatherRecord::parseOwiGridLine(
    const std::string &gridline) {
  std::string ilat, ilon, dx, dy, swlat, swlon, dt;
  ilat = gridline.substr(5, 4);
  ilon = gridline.substr(15, 4);
  dx = gridline.substr(22, 6);
  dy = gridline.substr(31, 6);
  swlat = gridline.substr(43, 8);
  swlon = gridline.substr(57, 8);
  dt = gridline.substr(68, 12);

  Grid g;
  g.nx = stoi(ilon);
  g.ny = stoi(ilat);
  g.dx = stod(dx);
  g.dy = stod(dy);
  g.xmin = stod(swlon);
  g.ymin = stod(swlat);
  g.xmax = g.x(g.nx - 1);
  g.ymax = g.y(g.ny - 1);
  g.time =
      Date(stoi(dt.substr(0, 4)), stoi(dt.substr(4, 2)), stoi(dt.substr(6, 2)),
           stoi(dt.substr(8, 2)), stoi(dt.substr(10, 2)), 0);

  return g;
}
