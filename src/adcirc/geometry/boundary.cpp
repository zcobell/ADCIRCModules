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
#include "adcirc/geometry/boundary.h"
#include "boost/format.hpp"

using namespace std;
using namespace Adcirc::Geometry;

/**
 * @name Boundary::Boundary
 * @brief Default constructor
 *
 * Initializes the boundary with code -1 and length 0.
 */
Boundary::Boundary() { this->setBoundary(-1, 0); }

/**
 * @overload Boundary::Boundary
 * @brief Initializes the boundary with user specified boundary type and length
 * @param boundaryCode ADCIRC model boundary code
 * @param boundaryLength number of nodes along this boundary
 */
Boundary::Boundary(int boundaryCode, size_t boundaryLength) {
  this->setBoundaryCode(boundaryCode);
  this->setBoundaryLength(boundaryLength);
}

/**
 * @name Boundary::setBoundary
 * @brief User specified initialization of the boundary
 * @param boundaryCode ADCIRC model boundary code
 * @param boundaryLength number of nodes along this boundary
 */
void Boundary::setBoundary(int boundaryCode, size_t boundaryLength) {
  this->setBoundaryCode(boundaryCode);
  this->setBoundaryLength(boundaryLength);
}

/**
 * @name Boundary::boundaryLength
 * @brief Returns the length of the boundary
 * @return number of nodes in boundary
 */
size_t Boundary::boundaryLength() const { return this->m_boundaryLength; }

/**
 * @name Boundary::size
 * @brief Returns the length of the boundary. Same as
 * Boundary::bonudaryLength
 * @return number of nodes in boundary
 */
size_t Boundary::size() const { return this->boundaryLength(); }

/**
 * @name Boundary::length
 * @brief Returns the length of the boundary. Same as
 * Boundary::bonudaryLength
 * @return number of nodes in boundary
 */
size_t Boundary::length() const { return this->boundaryLength(); }

/**
 * @name Boundary::setBoundaryLength
 * @brief Allocates the arrays used by the bounary
 * @param boundaryLength number of nodes along the boundary
 */
void Boundary::setBoundaryLength(size_t boundaryLength) {
  if (this->boundaryLength() != boundaryLength) {
    this->m_boundaryLength = boundaryLength;
    this->m_node1.resize(this->boundaryLength());

    if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
        this->boundaryCode() == 23) {
      this->m_crestElevation.resize(this->boundaryLength());
      this->m_supercriticalWeirCoefficient.resize(this->boundaryLength());
    } else if (this->boundaryCode() == 4 || this->boundaryCode() == 24) {
      this->m_node2.resize(this->boundaryLength());
      this->m_crestElevation.resize(this->boundaryLength());
      this->m_supercriticalWeirCoefficient.resize(this->boundaryLength());
      this->m_subcriticalWeirCoefficient.resize(this->boundaryLength());
    } else if (this->boundaryCode() == 5 || this->boundaryCode() == 25) {
      this->m_node2.resize(this->boundaryLength());
      this->m_crestElevation.resize(this->boundaryLength());
      this->m_supercriticalWeirCoefficient.resize(this->boundaryLength());
      this->m_subcriticalWeirCoefficient.resize(this->boundaryLength());
      this->m_pipeHeight.resize(this->boundaryLength());
      this->m_pipeDiameter.resize(this->boundaryLength());
      this->m_pipeCoefficient.resize(this->boundaryLength());
    }
  }
}

/**
 * @name Boundary::boundaryCode
 * @brief Returns the Adcirc model boundary code
 * @return Adcirc model boundary code
 */
int Boundary::boundaryCode() const { return this->m_boundaryCode; }

/**
 * @name Boundary::setBoundaryCode
 * @brief Sets the model boundary to the user specified code
 * @param boundaryCode Adcirc model boundary code
 */
void Boundary::setBoundaryCode(int boundaryCode) {
  this->m_boundaryCode = boundaryCode;
}

/**
 * @name Boundary::crestElevation
 * @brief Returns the crest elevation for boundary types 3, 13, 23, 4, 24, 5,
 * and 25
 * @param index position along the boundary
 * @return crest elevation if applicable, otherwise -9999.0.
 */
double Boundary::crestElevation(size_t index) const {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_crestElevation[index];
  return -9999.0;
}

/**
 * @name Boundary::setCrestElevation
 * @brief Sets the crest elevation for boundary types 3, 13, 23, 4, 24, 5, and
 * 25
 * @param index position along the boundary
 * @param crestElevation height above the datum for the weir crest
 */
void Boundary::setCrestElevation(size_t index, double crestElevation) {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_crestElevation[index] = crestElevation;
}

/**
 * @name Boundary::subcriticalWeirCoefficient
 * @brief Returns the coeffieicnt of subcritical weir flow for boundary types 4,
 * 24, 5, and 25
 * @param index position along the boundary
 * @return subcritical weir coefficient
 */
double Boundary::subcriticalWeirCoefficient(size_t index) const {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_subcriticalWeirCoefficient[index];
  return -9999.0;
}
/**
 * @name Boundary::setSubcriticalWeirCoefficient
 * @brief Sets the coefficient of subcritical flow for boundary types 4, 24, 5,
 * and 25
 * @param index position along the boundary
 * @param subcriticalWeirCoefficient coefficient of subcritical flow
 */
void Boundary::setSubcriticalWeirCoefficient(size_t index, double subcriticalWeirCoefficient) {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_subcriticalWeirCoefficient[index] = subcriticalWeirCoefficient;
}

/**
 * @name Boundary::supercriticalWeirCoefficient
 * @brief Returns the coefficient of supercritical flow for the specified
 * position along the boundary for boundary types 3, 13, 23, 4, 24, 5, and 25
 * @param index position along the boundary
 * @return coefficient of supercritical flow
 */
double Boundary::supercriticalWeirCoefficient(size_t index) const {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_supercriticalWeirCoefficient[index];
  return -9999.0;
}

/**
 * @name Boundary::setSupercriticalWeirCoefficient
 * @param index position along the boundary
 * @param supercriticalWeirCoefficient coefficient of supercritical flow for
 * boundarytypes 3, 13, 23, 4, 24, 5, and 25
 */
void Boundary::setSupercriticalWeirCoefficient(
    size_t index, double supercriticalWeirCoefficient) {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_supercriticalWeirCoefficient[index] =
          supercriticalWeirCoefficient;
}

/**
 * @name Boundary::pipeHeight
 * @brief Returns the elevation of the pipe above datum for type 5 and 25
 * boundaries
 * @param index position along the boundary
 * @return height of pipe center above datum
 */
double Boundary::pipeHeight(size_t index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeHeight[index];
  return -9999.0;
}

/**
 * @name Boundary::setPipeHeight
 * @brief Sets the elevation of the pipe above datum for type 5 and 25
 * boundaries
 * @param index position along the boundary
 * @param pipeHeight elevation of the pipe center above datum
 */
void Boundary::setPipeHeight(size_t index, double pipeHeight) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeHeight[index] = pipeHeight;
}

/**
 * @name Boundary::pipeDiameter
 * @brief Returns the diameter of the pipe
 * @param index position along the boundary
 * @return diameter of the pipe
 */
double Boundary::pipeDiameter(size_t index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeDiameter[index];
  return -9999.0;
}

/**
 * @name Boundary::setPipeDiameter
 * @brief Sets the diameter of the pipe
 * @param index postion along the boundary
 * @param pipeDiameter diameter of the pipe
 */
void Boundary::setPipeDiameter(size_t index, double pipeDiameter) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeDiameter[index] = pipeDiameter;
}

/**
 * @name Boundary::pipeCoefficient
 * @brief Returns the pipe coefficient
 * @param index position along the boundary
 * @return pipe coefficient
 */
double Boundary::pipeCoefficient(size_t index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeCoefficient[index];
  return -9999.0;
}

/**
 * @name Boundary::setPipeCoefficient
 * @brief Sets the pipe coefficient
 * @param index position along boundary
 * @param pipeCoefficient pipe coefficient
 */
void Boundary::setPipeCoefficient(size_t index, double pipeCoefficient) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeCoefficient[index] = pipeCoefficient;
}

/**
 * @name Boundary::node1
 * @brief Returns a pointer to the node on the boundary
 * @param index position along the boundary
 * @return pointer to an Node object
 */
Node *Boundary::node1(size_t index) const {
  if (index < this->boundaryLength() || index >= 0) return this->m_node1[index];
  return nullptr;
}

/**
 * @name Boundary::setNode1
 * @brief Sets a pointer to a node
 * @param index position along the boundary
 * @param node1 Pointer to an Node object
 */
void Boundary::setNode1(size_t index, Node *node1) {
  if (index < this->boundaryLength() || index >= 0)
    this->m_node1[index] = node1;
}

/**
 * @name Boundary::node2
 * @brief Returns a pointer to the paired node for type 4, 24, 5, and 25
 * boundaries
 * @param index position along the boundary
 * @return Node pointer
 */
Node *Boundary::node2(size_t index) const {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_node2[index];
  return nullptr;
}

/**
 * @name Boundary::setNode2
 * @brief Sets the node pair for type 4, 24, 5, and 25 boundaries
 * @param index position along the boundary
 * @param node2 pointer to an Node object
 */
void Boundary::setNode2(size_t index, Node *node2) {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_node2[index] = node2;
}

/**
 * @name Boundary::toStringList
 * @brief Writes the boundary into Adcirc ASCII mesh format and stores into a
 * string vector
 * @return vector of strings
 */
vector<string> Boundary::toStringList() {
  vector<string> outputList;

  if (this->boundaryCode() == -1)
    outputList.push_back(boost::str(boost::format("%11i") % this->length()));
  else
    outputList.push_back(boost::str(boost::format("%11i %11i") %
                                    this->length() % this->boundaryCode()));

  for (int i = 0; i < this->length(); i++) {
    if (this->boundaryCode() == 0 || this->boundaryCode() == 1 ||
        this->boundaryCode() == 2 || this->boundaryCode() == 10 ||
        this->boundaryCode() == 11 || this->boundaryCode() == 12 ||
        this->boundaryCode() == 20 || this->boundaryCode() == 21 ||
        this->boundaryCode() == 22 || this->boundaryCode() == 30 ||
        this->boundaryCode() == 52 || this->boundaryCode() == 102 ||
        this->boundaryCode() == 112 || this->boundaryCode() == 122) {
      outputList.push_back(
          boost::str(boost::format("%11i") % this->node1(i)->id()));
    } else if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
               this->boundaryCode() == 23) {
      outputList.push_back(boost::str(
          boost::format("%11i %6.3f %6.3f") % this->node1(i)->id() %
          this->crestElevation(i) % this->supercriticalWeirCoefficient(i)));
    } else if (this->boundaryCode() == 4 || this->boundaryCode() == 24) {
      outputList.push_back(boost::str(
          boost::format("%11i %11i %6.3f %6.3f %6.3f") % this->node1(i)->id() %
          this->node2(i)->id() % this->crestElevation(i) %
          this->subcriticalWeirCoefficient(i) %
          this->supercriticalWeirCoefficient(i)));
    } else if (this->boundaryCode() == 5 || this->boundaryCode() == 25) {
      outputList.push_back(boost::str(
          boost::format("%11i %11i %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f") %
          this->node1(i)->id() % this->node2(i)->id() %
          this->crestElevation(i) % this->subcriticalWeirCoefficient(i) %
          this->supercriticalWeirCoefficient(i) % this->pipeHeight(i) %
          this->pipeCoefficient(i) % this->pipeDiameter(i)));
    } else {
      outputList.push_back(
          boost::str(boost::format("%11i") % this->node1(i)->id()));
    }
  }
  return outputList;
}
