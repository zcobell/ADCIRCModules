#include "oceanweatherheader.h"
#include <fstream>

OceanweatherHeader::OceanweatherHeader() {}

void OceanweatherHeader::read(std::fstream *fid) {
  std::string line;
  std::getline(*(fid), line);
  auto sdate1 = std::string(line.begin() + 55, line.begin() + 65);
  auto sdate2 = std::string(line.begin() + 70, line.begin() + 80);

  this->m_startDate =
      Date(stoi(sdate1.substr(0, 4)), stoi(sdate1.substr(4, 2)),
           stoi(sdate1.substr(6, 2)), stoi(sdate1.substr(8, 2)), 0, 0);
  this->m_endDate =
      Date(stoi(sdate2.substr(0, 4)), stoi(sdate2.substr(4, 2)),
           stoi(sdate2.substr(6, 2)), stoi(sdate2.substr(8, 2)), 0, 0);
}

Date OceanweatherHeader::startDate() const { return m_startDate; }

void OceanweatherHeader::setStartDate(const Date &startDate) {
  m_startDate = startDate;
}

Date OceanweatherHeader::endDate() const { return m_endDate; }

void OceanweatherHeader::setEndDate(const Date &endDate) {
  m_endDate = endDate;
}

bool OceanweatherHeader::operator==(const OceanweatherHeader &b) const {
  return m_startDate == b.startDate() && m_endDate == b.endDate();
}

bool OceanweatherHeader::operator!=(const OceanweatherHeader &b) const {
  return !(*(this) == b);
}
