#ifndef OCEANWEATHERHEADER_H
#define OCEANWEATHERHEADER_H

#include "date.h"

class OceanweatherHeader {
 public:
  OceanweatherHeader();

  void read(std::fstream *fid);

  Date startDate() const;
  void setStartDate(const Date &startDate);

  Date endDate() const;
  void setEndDate(const Date &endDate);

  bool operator==(const OceanweatherHeader &b) const;
  bool operator!=(const OceanweatherHeader &b) const;

 private:
  Date m_startDate;
  Date m_endDate;
};

#endif  // OCEANWEATHERHEADER_H
