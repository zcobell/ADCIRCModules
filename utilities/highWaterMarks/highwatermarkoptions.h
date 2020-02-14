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
#ifndef HIGHWATERMARKOPTIONS_H
#define HIGHWATERMARKOPTIONS_H

#include <string>
#include "cxxopts.hpp"

class HighWaterMarkOptions {
 public:
  HighWaterMarkOptions();
  HighWaterMarkOptions(const cxxopts::ParseResult &parser);

  std::string mesh() const;
  void setMesh(const std::string &mesh);

  std::string maxele() const;
  void setMaxele(const std::string &maxele);

  std::string station() const;
  void setStation(const std::string &station);

  std::string output() const;
  void setOutput(const std::string &output);

  double distance() const;
  void setDistance(double distance);

  bool stats() const;
  void setStats(bool stats);

  bool zero() const;
  void setZero(bool zero);

  std::string field() const;
  void setField(const std::string &field);

  size_t searchDepth() const;
  void setSearchDepth(const size_t &searchDepth);

  double adcircMultiplier() const;
  void setAdcircMultiplier(double multiplier);

  double dataMultiplier() const;
  void setDataMultiplier(double multiplier);

private:
  std::string m_mesh;
  std::string m_maxele;
  std::string m_station;
  std::string m_output;
  double m_distance;
  double m_adcircMultiplier;
  double m_dataMultiplier;
  bool m_stats;
  bool m_zero;
  std::string m_field;
  size_t m_searchDepth;
};

#endif  // HIGHWATERMARKOPTIONS_H
