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
#ifndef ADCMOD_NODE_H
#define ADCMOD_NODE_H

#include <string>
#include <utility>
#include "adcircmodules_global.h"
#include "hashtype.h"

using Point = std::pair<double, double>;

namespace Adcirc {
namespace Geometry {

/**
 * @class Node
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief The Node class describes the x, y, z position of a single mesh
 * node
 *
 */

class Node {
 public:
  ADCIRCMODULES_EXPORT Node();
  ADCIRCMODULES_EXPORT Node(size_t id, double x, double y, double z);

  ADCIRCMODULES_EXPORT ~Node();

  void ADCIRCMODULES_EXPORT setNode(size_t id, double x, double y, double z);

  double ADCIRCMODULES_EXPORT x() const;
  void ADCIRCMODULES_EXPORT setX(double x);

  double ADCIRCMODULES_EXPORT y() const;
  void ADCIRCMODULES_EXPORT setY(double y);

  double ADCIRCMODULES_EXPORT z() const;
  void ADCIRCMODULES_EXPORT setZ(double z);

  size_t ADCIRCMODULES_EXPORT id() const;
  void ADCIRCMODULES_EXPORT setId(size_t id);

  std::string ADCIRCMODULES_EXPORT toAdcircString(bool geographicCoordinates);
  std::string ADCIRCMODULES_EXPORT to2dmString(bool geographicCoordinates);

  Point ADCIRCMODULES_EXPORT toPoint();

  std::string ADCIRCMODULES_EXPORT
  hash(Adcirc::Cryptography::HashType h =
           Adcirc::Cryptography::AdcircDefaultHash,
       bool force = false);

  std::string ADCIRCMODULES_EXPORT
  positionHash(Adcirc::Cryptography::HashType h =
                   Adcirc::Cryptography::AdcircDefaultHash,
               bool force = false);

 private:
  size_t m_id;           /// Integer name of a mesh node
  double m_x;            /// x position of a node
  double m_y;            /// y position of a node
  double m_z;            /// z position of a node
  char* m_hash;          /// hash identifier for the node
  char* m_positionHash;  /// has of only node's position

  void generateHash(Adcirc::Cryptography::HashType h =
                        Adcirc::Cryptography::AdcircDefaultHash);
  void generatePositionHash(Adcirc::Cryptography::HashType h =
                                Adcirc::Cryptography::AdcircDefaultHash);
};
}  // namespace Geometry
}  // namespace Adcirc
#endif  // ADCMOD_NODE_H
