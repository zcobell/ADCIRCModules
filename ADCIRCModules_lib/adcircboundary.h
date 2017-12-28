//------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
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
//------------------------------------------------------------------------//
#ifndef ADCIRCBOUNDARY_H
#define ADCIRCBOUNDARY_H

#include "adcircmodules_global.h"
#include "adcircnode.h"
#include <string>
#include <vector>

class AdcircBoundary {
public:
  explicit AdcircBoundary();

  explicit AdcircBoundary(int boundaryCode, int boundaryLength);

  void setBoundary(int boundaryCode, int boundaryLength);

  int boundaryCode() const;
  void setBoundaryCode(int boundaryCode);

  int boundaryLength() const;
  void setBoundaryLength(int boundaryLength);
  int size() const;
  int length() const;

  double crestElevation(int index) const;
  void setCrestElevation(int index, double crestElevation);

  double subcriticalWeirCoeffient(int index) const;
  void setSubcriticalWeirCoeffient(int index, double subcriticalWeirCoeffient);

  double supercriticalWeirCoefficient(int index) const;
  void setSupercriticalWeirCoefficient(int index,
                                       double supercriticalWeirCoefficient);

  double pipeHeight(int index) const;
  void setPipeHeight(int index, double pipeHeight);

  double pipeDiameter(int index) const;
  void setPipeDiameter(int index, double pipeDiameter);

  double pipeCoefficient(int index) const;
  void setPipeCoefficient(int index, double pipeCoefficient);

  AdcircNode *node1(int index) const;
  void setNode1(int index, AdcircNode *node1);

  AdcircNode *node2(int index) const;
  void setNode2(int index, AdcircNode *node2);

  std::vector<std::string> toStringList();

private:
  int m_boundaryCode;
  int m_boundaryLength;
  std::vector<double> m_crestElevation;
  std::vector<double> m_subcriticalWeirCoeffient;
  std::vector<double> m_supercriticalWeirCoefficient;
  std::vector<double> m_pipeHeight;
  std::vector<double> m_pipeDiameter;
  std::vector<double> m_pipeCoefficient;
  std::vector<double> m_averageLongitude;
  std::vector<AdcircNode *> m_node1;
  std::vector<AdcircNode *> m_node2;
};

#endif // ADCIRCBOUNDARY_H
