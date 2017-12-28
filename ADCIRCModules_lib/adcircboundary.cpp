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
#include "adcircboundary.h"
#include "boost/format.hpp"

/**
 * @name AdcircBoundary::AdcircBoundary
 * @brief Default constructor
 *
 * Initializes the boundary with code -1 and length 0.
 */
AdcircBoundary::AdcircBoundary() { this->setBoundary(-1, 0); }

/**
 * @overload AdcircBoundary::AdcircBoundary
 * @brief Initializes the boundary with user specified boundary type and length
 * @param boundaryCode ADCIRC model boundary code
 * @param boundaryLength number of nodes along this boundary
 */
AdcircBoundary::AdcircBoundary(int boundaryCode, int boundaryLength) {
  this->setBoundaryCode(boundaryCode);
  this->setBoundaryLength(boundaryLength);
}

/**
 * @name AdcircBoundary::setBoundary
 * @brief User specified initialization of the boundary
 * @param boundaryCode ADCIRC model boundary code
 * @param boundaryLength number of nodes along this boundary
 */
void AdcircBoundary::setBoundary(int boundaryCode, int boundaryLength) {
  this->setBoundaryCode(boundaryCode);
  this->setBoundaryLength(boundaryLength);
}

/**
 * @name AdcircBoundary::boundaryLength
 * @brief Returns the length of the boundary
 * @return number of nodes in boundary
 */
int AdcircBoundary::boundaryLength() const { return this->m_boundaryLength; }

/**
 * @name AdcircBoundary::size
 * @brief Returns the length of the boundary. Same as
 * AdcircBoundary::bonudaryLength
 * @return number of nodes in boundary
 */
int AdcircBoundary::size() const { return this->boundaryLength(); }

/**
 * @name AdcircBoundary::length
 * @brief Returns the length of the boundary. Same as
 * AdcircBoundary::bonudaryLength
 * @return number of nodes in boundary
 */
int AdcircBoundary::length() const { return this->boundaryLength(); }

/**
 * @name AdcircBoundary::setBoundaryLength
 * @brief Allocates the arrays used by the bounary
 * @param boundaryLength number of nodes along the boundary
 */
void AdcircBoundary::setBoundaryLength(int boundaryLength) {
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
 * @name AdcircBoundary::boundaryCode
 * @brief Returns the Adcirc model boundary code
 * @return Adcirc model boundary code
 */
int AdcircBoundary::boundaryCode() const { return this->m_boundaryCode; }

/**
 * @name AdcircBoundary::setBoundaryCode
 * @brief Sets the model boundary to the user specified code
 * @param boundaryCode Adcirc model boundary code
 */
void AdcircBoundary::setBoundaryCode(int boundaryCode) {
  this->m_boundaryCode = boundaryCode;
}

/**
 * @name AdcircBoundary::crestElevation
 * @brief Returns the crest elevation for boundary types 3, 13, 23, 4, 24, 5,
 * and 25
 * @param index position along the boundary
 * @return crest elevation if applicable, otherwise -9999.0.
 */
double AdcircBoundary::crestElevation(int index) const {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_crestElevation[index];
  return -9999.0;
}

/**
 * @name AdcircBoundary::setCrestElevation
 * @brief Sets the crest elevation for boundary types 3, 13, 23, 4, 24, 5, and
 * 25
 * @param index position along the boundary
 * @param crestElevation height above the datum for the weir crest
 */
void AdcircBoundary::setCrestElevation(int index, double crestElevation) {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_crestElevation[index] = crestElevation;
}

/**
 * @name AdcircBoundary::subcriticalWeirCoefficient
 * @brief Returns the coeffieicnt of subcritical weir flow for boundary types 4,
 * 24, 5, and 25
 * @param index position along the boundary
 * @return subcritical weir coefficient
 */
double AdcircBoundary::subcriticalWeirCoefficient(int index) const {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_subcriticalWeirCoefficient[index];
  return -9999.0;
}
/**
 * @name AdcircBoundary::setSubcriticalWeirCoefficient
 * @brief Sets the coefficient of subcritical flow for boundary types 4, 24, 5,
 * and 25
 * @param index position along the boundary
 * @param subcriticalWeirCoefficient coefficient of subcritical flow
 */
void AdcircBoundary::setSubcriticalWeirCoefficient(
    int index, double subcriticalWeirCoefficient) {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_subcriticalWeirCoefficient[index] = subcriticalWeirCoefficient;
}

/**
 * @name AdcircBoundary::supercriticalWeirCoefficient
 * @brief Returns the coefficient of supercritical flow for the specified
 * position along the boundary for boundary types 3, 13, 23, 4, 24, 5, and 25
 * @param index position along the boundary
 * @return coefficient of supercritical flow
 */
double AdcircBoundary::supercriticalWeirCoefficient(int index) const {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_supercriticalWeirCoefficient[index];
  return -9999.0;
}

/**
 * @name AdcircBoundary::setSupercriticalWeirCoefficient
 * @param index position along the boundary
 * @param supercriticalWeirCoefficient coefficient of supercritical flow for
 * boundarytypes 3, 13, 23, 4, 24, 5, and 25
 */
void AdcircBoundary::setSupercriticalWeirCoefficient(
    int index, double supercriticalWeirCoefficient) {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_supercriticalWeirCoefficient[index] =
          supercriticalWeirCoefficient;
}

/**
 * @name AdcircBoundary::pipeHeight
 * @brief Returns the elevation of the pipe above datum for type 5 and 25
 * boundaries
 * @param index position along the boundary
 * @return height of pipe center above datum
 */
double AdcircBoundary::pipeHeight(int index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeHeight[index];
  return -9999.0;
}

/**
 * @name AdcircBoundary::setPipeHeight
 * @brief Sets the elevation of the pipe above datum for type 5 and 25
 * boundaries
 * @param index position along the boundary
 * @param pipeHeight elevation of the pipe center above datum
 */
void AdcircBoundary::setPipeHeight(int index, double pipeHeight) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeHeight[index] = pipeHeight;
}

/**
 * @name AdcircBoundary::pipeDiameter
 * @brief Returns the diameter of the pipe
 * @param index position along the boundary
 * @return diameter of the pipe
 */
double AdcircBoundary::pipeDiameter(int index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeDiameter[index];
  return -9999.0;
}

/**
 * @name AdcircBoundary::setPipeDiameter
 * @brief Sets the diameter of the pipe
 * @param index postion along the boundary
 * @param pipeDiameter diameter of the pipe
 */
void AdcircBoundary::setPipeDiameter(int index, double pipeDiameter) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeDiameter[index] = pipeDiameter;
}

/**
 * @name AdcircBoundary::pipeCoefficient
 * @brief Returns the pipe coefficient
 * @param index position along the boundary
 * @return pipe coefficient
 */
double AdcircBoundary::pipeCoefficient(int index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeCoefficient[index];
  return -9999.0;
}

/**
 * @name AdcircBoundary::setPipeCoefficient
 * @brief Sets the pipe coefficient
 * @param index position along boundary
 * @param pipeCoefficient pipe coefficient
 */
void AdcircBoundary::setPipeCoefficient(int index, double pipeCoefficient) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeCoefficient[index] = pipeCoefficient;
}

/**
 * @name AdcircBoundary::node1
 * @brief Returns a pointer to the node on the boundary
 * @param index position along the boundary
 * @return pointer to an AdcircNode object
 */
AdcircNode *AdcircBoundary::node1(int index) const {
  if (index < this->boundaryLength() || index >= 0)
    return this->m_node1[index];
  return nullptr;
}

/**
 * @name AdcircBoundary::setNode1
 * @brief Sets a pointer to a node
 * @param index position along the boundary
 * @param node1 Pointer to an AdcircNode object
 */
void AdcircBoundary::setNode1(int index, AdcircNode *node1) {
  if (index < this->boundaryLength() || index >= 0)
    this->m_node1[index] = node1;
}

/**
 * @name AdcircBoundary::node2
 * @brief Returns a pointer to the paired node for type 4, 24, 5, and 25
 * boundaries
 * @param index position along the boundary
 * @return AdcircNode pointer
 */
AdcircNode *AdcircBoundary::node2(int index) const {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_node2[index];
  return nullptr;
}

/**
 * @name AdcircBoundary::setNode2
 * @brief Sets the node pair for type 4, 24, 5, and 25 boundaries
 * @param index position along the boundary
 * @param node2 pointer to an AdcircNode object
 */
void AdcircBoundary::setNode2(int index, AdcircNode *node2) {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_node2[index] = node2;
}

/**
 * @name AdcircBoundary::toStringList
 * @brief Writes the boundary into Adcirc ASCII mesh format and stores into a
 * string vector
 * @return vector of strings
 */
std::vector<std::string> AdcircBoundary::toStringList() {
  std::vector<std::string> outputList;

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
