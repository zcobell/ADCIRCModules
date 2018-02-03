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
/**
 * @class AdcircBoundary
 * @author Zachary Cobell
 *
 * The AdcircBoundary class handles the interpretation of boundary
 * conditions used within the Adcirc model. The class is designed so that
 * it is generic and can handle both open and land boundaries.
 *
 */
#ifndef ADCIRCBOUNDARY_H
#define ADCIRCBOUNDARY_H

#include "adcirc/adcircmodules_global.h"
#include "adcirc/geometry/node.h"
#include <string>
#include <vector>

using namespace std;

namespace Adcirc {
namespace Geometry {

class Boundary {
public:
  explicit Boundary();

  explicit Boundary(int boundaryCode, int boundaryLength);

  void setBoundary(int boundaryCode, int boundaryLength);

  int boundaryCode() const;
  void setBoundaryCode(int boundaryCode);

  int boundaryLength() const;
  void setBoundaryLength(int boundaryLength);
  int size() const;
  int length() const;

  double crestElevation(int index) const;
  void setCrestElevation(int index, double crestElevation);

  double subcriticalWeirCoefficient(int index) const;
  void setSubcriticalWeirCoefficient(int index,
                                     double subcriticalWeirCoefficient);

  double supercriticalWeirCoefficient(int index) const;
  void setSupercriticalWeirCoefficient(int index,
                                       double supercriticalWeirCoefficient);

  double pipeHeight(int index) const;
  void setPipeHeight(int index, double pipeHeight);

  double pipeDiameter(int index) const;
  void setPipeDiameter(int index, double pipeDiameter);

  double pipeCoefficient(int index) const;
  void setPipeCoefficient(int index, double pipeCoefficient);

  Adcirc::Geometry::Node *node1(int index) const;
  void setNode1(int index, Adcirc::Geometry::Node *node1);

  Adcirc::Geometry::Node *node2(int index) const;
  void setNode2(int index, Adcirc::Geometry::Node *node2);

  vector<string> toStringList();

private:
  int m_boundaryCode;
  int m_boundaryLength;
  vector<double> m_crestElevation;
  vector<double> m_subcriticalWeirCoefficient;
  vector<double> m_supercriticalWeirCoefficient;
  vector<double> m_pipeHeight;
  vector<double> m_pipeDiameter;
  vector<double> m_pipeCoefficient;
  vector<double> m_averageLongitude;
  vector<Adcirc::Geometry::Node *> m_node1;
  vector<Adcirc::Geometry::Node *> m_node2;
};
}
}
#endif // ADCIRCBOUNDARY_H
