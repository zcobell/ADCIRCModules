/*------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------*/
#ifndef ADCMOD_BOUNDARY_H
#define ADCMOD_BOUNDARY_H

#include <string>
#include <vector>
#include "adcircmodules_global.h"
#include "node.h"

namespace Adcirc {
namespace Geometry {

/**
 * @class Boundary
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief The boundary class handles ADCIRC mesh boundaries
 *
 * The Boundary class handles the interpretation of boundary
 * conditions used within the Adcirc model. The class is designed so that
 * it is generic and can handle both open and land boundaries.
 *
 */

class Boundary {
 public:
  ADCIRCMODULES_EXPORT Boundary();

  ADCIRCMODULES_EXPORT Boundary(int boundaryCode, size_t boundaryLength);

  ADCIRCMODULES_EXPORT ~Boundary();

  void ADCIRCMODULES_EXPORT setBoundary(int boundaryCode,
                                        size_t boundaryLength);

  int ADCIRCMODULES_EXPORT boundaryCode() const;
  void ADCIRCMODULES_EXPORT setBoundaryCode(int boundaryCode);

  size_t ADCIRCMODULES_EXPORT boundaryLength() const;
  void ADCIRCMODULES_EXPORT setBoundaryLength(size_t boundaryLength);
  size_t ADCIRCMODULES_EXPORT size() const;
  size_t ADCIRCMODULES_EXPORT length() const;

  double ADCIRCMODULES_EXPORT crestElevation(size_t index) const;
  void ADCIRCMODULES_EXPORT setCrestElevation(size_t index,
                                              double crestElevation);

  double ADCIRCMODULES_EXPORT subcriticalWeirCoefficient(size_t index) const;
  void ADCIRCMODULES_EXPORT setSubcriticalWeirCoefficient(
      size_t index, double subcriticalWeirCoefficient);

  double ADCIRCMODULES_EXPORT supercriticalWeirCoefficient(size_t index) const;
  void ADCIRCMODULES_EXPORT setSupercriticalWeirCoefficient(
      size_t index, double supercriticalWeirCoefficient);

  double ADCIRCMODULES_EXPORT pipeHeight(size_t index) const;
  void ADCIRCMODULES_EXPORT setPipeHeight(size_t index, double pipeHeight);

  double ADCIRCMODULES_EXPORT pipeDiameter(size_t index) const;
  void ADCIRCMODULES_EXPORT setPipeDiameter(size_t index, double pipeDiameter);

  double ADCIRCMODULES_EXPORT pipeCoefficient(size_t index) const;
  void ADCIRCMODULES_EXPORT setPipeCoefficient(size_t index,
                                               double pipeCoefficient);

  Adcirc::Geometry::Node ADCIRCMODULES_EXPORT *node1(size_t index) const;
  void ADCIRCMODULES_EXPORT setNode1(size_t index,
                                     Adcirc::Geometry::Node *node1);

  Adcirc::Geometry::Node ADCIRCMODULES_EXPORT *node2(size_t index) const;
  void ADCIRCMODULES_EXPORT setNode2(size_t index,
                                     Adcirc::Geometry::Node *node2);

  std::vector<std::string> ADCIRCMODULES_EXPORT toStringList();

  bool ADCIRCMODULES_EXPORT isWeir() const;
  bool ADCIRCMODULES_EXPORT isInternalWeir() const;
  bool ADCIRCMODULES_EXPORT isInternalWeirWithoutPipes() const;
  bool ADCIRCMODULES_EXPORT isExternalWeir() const;
  bool ADCIRCMODULES_EXPORT isInternalWeirWithPipes() const;
  bool ADCIRCMODULES_EXPORT isSingleNodeBoundary() const;

  double ADCIRCMODULES_EXPORT averageLongitude(bool force = false);

  std::string ADCIRCMODULES_EXPORT hash(HashType h = AdcircDefaultHash,
                                        bool force = false);

 private:
  int m_boundaryCode;
  size_t m_boundaryLength;
  std::vector<double> m_crestElevation;
  std::vector<double> m_subcriticalWeirCoefficient;
  std::vector<double> m_supercriticalWeirCoefficient;
  std::vector<double> m_pipeHeight;
  std::vector<double> m_pipeDiameter;
  std::vector<double> m_pipeCoefficient;
  double m_averageLongitude;
  std::vector<Adcirc::Geometry::Node *> m_node1;
  std::vector<Adcirc::Geometry::Node *> m_node2;
  char *m_hash;

  void generateHash(HashType h = AdcircDefaultHash);
  void calculateAverageLongitude();
};
}  // namespace Geometry
}  // namespace Adcirc
#endif  // ADCMOD_BOUNDARY_H
