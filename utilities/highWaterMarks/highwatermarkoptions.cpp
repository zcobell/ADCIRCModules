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
#include "highwatermarkoptions.h"

HighWaterMarkOptions::HighWaterMarkOptions()
    : m_mesh(std::string()),
      m_maxele(std::string()),
      m_station(std::string()),
      m_output(std::string()),
      m_distance(0),
      m_stats(false),
      m_zero(false),
      m_field(std::string()),
      m_adcircMultiplier(1.0),
      m_dataMultiplier(1.0),
      m_searchDepth(500) {}

HighWaterMarkOptions::HighWaterMarkOptions(const cxxopts::ParseResult &parser) {
  if (parser["mesh"].count()) {
    this->m_mesh = parser["mesh"].as<std::string>();
  }
  if (parser["maxele"].count()) {
    this->m_maxele = parser["maxele"].as<std::string>();
  }
  if (parser["station"].count()) {
    this->m_station = parser["station"].as<std::string>();
  }
  if (parser["output"].count()) {
    this->m_output = parser["output"].as<std::string>();
  }
  if (parser["distance"].count()) {
    this->m_distance = parser["distance"].as<double>();
  }
  if (parser["stats"].count()) {
    this->m_stats = parser["stats"].as<bool>();
  }
  if (parser["zero"].count()) {
    this->m_zero = true;
  } else {
    this->m_zero = false;
  }
  if (parser["field"].count()) {
    this->m_field = parser["field"].as<std::string>();
  } else {
    this->m_field = "observed";
  }
  if (parser["sdepth"].count()) {
    m_searchDepth = parser["sdepth"].as<size_t>();
  }
  if (parser["cadcirc"].count()) {
      m_adcircMultiplier = parser["cadcirc"].as<double>();
  } else {
      m_adcircMultiplier = 1.0;
  }
  if (parser["cobs"].count()) {
      m_dataMultiplier = parser["cobs"].as<double>();
  } else {
      m_dataMultiplier = 1.0;
  }
}

std::string HighWaterMarkOptions::mesh() const { return m_mesh; }

void HighWaterMarkOptions::setMesh(const std::string &mesh) { m_mesh = mesh; }

std::string HighWaterMarkOptions::maxele() const { return m_maxele; }

void HighWaterMarkOptions::setMaxele(const std::string &maxele) {
  m_maxele = maxele;
}

std::string HighWaterMarkOptions::station() const { return m_station; }

void HighWaterMarkOptions::setStation(const std::string &station) {
  m_station = station;
}

std::string HighWaterMarkOptions::output() const { return m_output; }

void HighWaterMarkOptions::setOutput(const std::string &output) {
  m_output = output;
}

double HighWaterMarkOptions::distance() const { return m_distance; }

void HighWaterMarkOptions::setDistance(double distance) {
  m_distance = distance;
}

bool HighWaterMarkOptions::stats() const { return m_stats; }

void HighWaterMarkOptions::setStats(bool stats) { m_stats = stats; }

bool HighWaterMarkOptions::zero() const { return m_zero; }

void HighWaterMarkOptions::setZero(bool zero) { m_zero = zero; }

std::string HighWaterMarkOptions::field() const { return m_field; }

void HighWaterMarkOptions::setField(const std::string &field) {
  m_field = field;
}

size_t HighWaterMarkOptions::searchDepth() const { return m_searchDepth; }

void HighWaterMarkOptions::setSearchDepth(const size_t &searchDepth) {
  m_searchDepth = searchDepth;
}

double HighWaterMarkOptions::adcircMultiplier() const {
    return this->m_adcircMultiplier;
}

double HighWaterMarkOptions::dataMultiplier() const {
    return this->m_dataMultiplier;
}
