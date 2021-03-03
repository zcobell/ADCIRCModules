#ifndef OCEANWEATHERRECORD_H
#define OCEANWEATHERRECORD_H

#include <fstream>
#include "date.h"

class OceanweatherRecord {
 public:
  OceanweatherRecord();

  void addDomain(std::fstream *fid_pressure, std::fstream *fid_wind);

  int read();

  struct Grid {
    double xmin;
    double ymin;
    double xmax;
    double ymax;
    double dx;
    double dy;
    size_t nx;
    size_t ny;
    Date time;

    double x(size_t i) const { return i < nx ? xmin + i * dx : 0.0; }
    double y(size_t j) const { return j < ny ? ymin + j * dy : 0.0; }

    bool operator==(const Grid &g) const {
      if (xmin != g.xmin) return false;
      if (xmax != g.xmax) return false;
      if (ymin != g.ymin) return false;
      if (ymax != g.ymax) return false;
      if (dx != g.dx) return false;
      if (dy != g.dy) return false;
      if (nx != g.nx) return false;
      if (ny != g.ny) return false;
      if (time != g.time) return false;
      return true;
    }

    bool operator!=(const Grid &g) { return !(*(this) == g); }
  };

  struct Domain {
    std::fstream *fid_pressure;
    std::fstream *fid_wind;
    std::vector<double> data_pressure;
    std::vector<double> data_u;
    std::vector<double> data_v;
    Grid grid;
  };

  size_t ndomain() const;

  Domain *domain(size_t index);

  std::tuple<double, double, double, size_t> get(const size_t i, const size_t j,
                                                 const size_t domain) const;

  std::tuple<double, double, double, size_t> get(
      const double x, const double y,
      size_t domain = std::numeric_limits<size_t>::max()) const;

  std::tuple<size_t, size_t, size_t, double, double, double, double>
  interpolationWeight(const double x, const double y,
                      size_t domain = std::numeric_limits<size_t>::max()) const;

  double backgroundPressure() const;
  void setBackgroundPressure(double backgroundPressure);

  bool gridChanged() const;

private:
  static OceanweatherRecord::Grid parseOwiGridLine(const std::string &gridline);
  static int readData(const Domain &d, std::fstream *fid,
                      std::vector<double> &array);
  bool inDomain(size_t domain, const double x, const double y) const;
  size_t findDomain(const double x, const double y) const;

  std::vector<Domain> m_domains;
  double m_backgroundPressure;
  bool m_gridchanged;
};

#endif  // OCEANWEATHERRECORD_H
