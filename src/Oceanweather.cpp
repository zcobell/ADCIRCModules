#include "oceanweather.h"
#include "logging.h"

Oceanweather::Oceanweather()
    : m_currentSnap(0), m_record(OceanweatherRecord()) {}

void Oceanweather::addDomain(std::string pressureFile, std::string windFile) {
  m_pressureFiles.push_back(std::move(pressureFile));
  m_windFiles.push_back(std::move(windFile));
}

size_t Oceanweather::nDomains() const {
  if (m_pressureFiles.size() == m_windFiles.size())
    return m_pressureFiles.size();
  oceanweather_throw_exception("Invalid state: npressure != nwind");
  return 0;
}

size_t Oceanweather::currentSnap() const { return m_currentSnap; }

int Oceanweather::open() {
  for (size_t i = 0; i < nDomains(); ++i) {
    m_fid_pressure.emplace_back(new std::fstream(m_pressureFiles[i]));
    if (!m_fid_pressure.back()->good()) {
      oceanweather_throw_exception("Could not find file " + m_pressureFiles[i]);
    }
    OceanweatherHeader preheader;
    preheader.read(m_fid_pressure.back().get());

    m_fid_wind.emplace_back(new std::fstream(m_windFiles[i]));
    if (!m_fid_wind.back()->good()) {
      oceanweather_throw_exception("Could not find file " + m_windFiles[i]);
    }
    OceanweatherHeader windheader;
    windheader.read(m_fid_wind.back().get());

    if (preheader != windheader) {
      oceanweather_throw_exception("The file headers for the domain " +
                                   std::to_string(i) +
                                   " do not have aligned dates.");
    }

    m_headers.push_back(preheader);
    m_record.addDomain(m_fid_pressure[i].get(), m_fid_wind[i].get());
  }

  return 0;
}

int Oceanweather::read() {
  this->m_record.read();
  return 0;
}

OceanweatherRecord Oceanweather::record() const
{
    return m_record;
}
