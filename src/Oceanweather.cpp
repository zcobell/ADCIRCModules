/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
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
#include "Oceanweather.h"

#include "FileIO.h"
#include "Logging.h"

using namespace Adcirc;

Oceanweather::Oceanweather()
    : m_currentSnap(0), m_isOpen(false), m_record(OceanweatherRecord()) {}

int Oceanweather::addDomain(std::string pressureFile, std::string windFile) {
  if (!Adcirc::FileIO::Generic::fileExists(pressureFile) ||
      !Adcirc::FileIO::Generic::fileExists(windFile)) {
    Adcirc::Logging::warning("Supplied files do not exist");
    return 1;
  }
  m_pressureFiles.push_back(std::move(pressureFile));
  m_windFiles.push_back(std::move(windFile));
  return 0;
}

size_t Oceanweather::nDomains() const {
  if (m_pressureFiles.size() == m_windFiles.size())
    return m_pressureFiles.size();
  adcircmodules_throw_exception("Invalid state: npressure != nwind");
  return 0;
}

size_t Oceanweather::currentSnap() const { return m_currentSnap; }

int Oceanweather::open() {
  for (size_t i = 0; i < nDomains(); ++i) {
    m_fid_pressure.emplace_back(new std::fstream(m_pressureFiles[i]));
    if (!m_fid_pressure.back()->good()) {
      adcircmodules_throw_exception("Could not find file " +
                                    m_pressureFiles[i]);
    }
    OceanweatherHeader preheader;
    preheader.read(m_fid_pressure.back().get());

    m_fid_wind.emplace_back(new std::fstream(m_windFiles[i]));
    if (!m_fid_wind.back()->good()) {
      adcircmodules_throw_exception("Could not find file " + m_windFiles[i]);
    }
    OceanweatherHeader windheader;
    windheader.read(m_fid_wind.back().get());

    if (preheader != windheader) {
      adcircmodules_throw_exception("The file headers for the domain " +
                                    std::to_string(i) +
                                    " do not have aligned dates.");
    }

    m_headers.push_back(preheader);
    m_record.addDomain(m_fid_pressure[i].get(), m_fid_wind[i].get());
  }
  m_isOpen = true;

  return 0;
}

int Oceanweather::read() {
  if (!m_isOpen) {
    this->open();
  }
  return this->m_record.read();
}

OceanweatherRecord Oceanweather::record() const { return m_record; }

std::string Oceanweather::windFile(int domain) const {
  return m_windFiles[domain];
}

std::string Oceanweather::pressureFile(int domain) const {
  return m_pressureFiles[domain];
}

int Oceanweather::close() {
  for (auto &f : m_fid_pressure) {
    f->close();
  }
  for (auto &f : m_fid_wind) {
    f->close();
  }
  return 0;
}
