#ifndef OCEANWEATHER_H
#define OCEANWEATHER_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "oceanweatherheader.h"
#include "oceanweatherrecord.h"

class Oceanweather {
 public:
  Oceanweather();

  void addDomain(std::string pressureFile, std::string windFile);

  size_t nDomains() const;

  size_t currentSnap() const;

  int open();

  int read();

  std::string pressureFile(int domain = 0) const;
  std::string windFile(int domain = 0) const;

  OceanweatherRecord record() const;

private:
  std::vector<std::string> m_pressureFiles;
  std::vector<std::string> m_windFiles;
  size_t m_currentSnap;
  bool m_isOpen;
  std::vector<std::unique_ptr<std::fstream>> m_fid_pressure;
  std::vector<std::unique_ptr<std::fstream>> m_fid_wind;
  std::vector<OceanweatherHeader> m_headers;
  OceanweatherRecord m_record;
};

#endif  // OCEANWEATHER_H
