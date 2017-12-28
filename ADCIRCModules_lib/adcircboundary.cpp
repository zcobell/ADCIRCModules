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

AdcircBoundary::AdcircBoundary() { this->setBoundary(0, 0); }

AdcircBoundary::AdcircBoundary(int boundaryCode, int boundaryLength) {
  this->setBoundaryCode(boundaryCode);
  this->setBoundaryLength(boundaryLength);
}

void AdcircBoundary::setBoundary(int boundaryCode, int boundaryLength) {
  this->setBoundaryCode(boundaryCode);
  this->setBoundaryLength(boundaryLength);
}

int AdcircBoundary::boundaryLength() const { return this->m_boundaryLength; }
int AdcircBoundary::size() const { return this->boundaryLength(); }
int AdcircBoundary::length() const { return this->boundaryLength(); }

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
      this->m_subcriticalWeirCoeffient.resize(this->boundaryLength());
    } else if (this->boundaryCode() == 5 || this->boundaryCode() == 25) {
      this->m_node2.resize(this->boundaryLength());
      this->m_crestElevation.resize(this->boundaryLength());
      this->m_supercriticalWeirCoefficient.resize(this->boundaryLength());
      this->m_subcriticalWeirCoeffient.resize(this->boundaryLength());
      this->m_pipeHeight.resize(this->boundaryLength());
      this->m_pipeDiameter.resize(this->boundaryLength());
      this->m_pipeCoefficient.resize(this->boundaryLength());
    }
  }
}

int AdcircBoundary::boundaryCode() const { return this->m_boundaryCode; }

void AdcircBoundary::setBoundaryCode(int boundaryCode) {
  this->m_boundaryCode = boundaryCode;
}

double AdcircBoundary::crestElevation(int index) const {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_crestElevation[index];
  return -9999.0;
}

void AdcircBoundary::setCrestElevation(int index, double crestElevation) {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_crestElevation[index] = crestElevation;
}

double AdcircBoundary::subcriticalWeirCoeffient(int index) const {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_subcriticalWeirCoeffient[index];
  return -9999.0;
}

void AdcircBoundary::setSubcriticalWeirCoeffient(
    int index, double subcriticalWeirCoeffient) {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_subcriticalWeirCoeffient[index] = subcriticalWeirCoeffient;
}

double AdcircBoundary::supercriticalWeirCoefficient(int index) const {
  if (this->boundaryCode() == 3 || this->boundaryCode() == 13 ||
      this->boundaryCode() == 23 || this->boundaryCode() == 4 ||
      this->boundaryCode() == 24 || this->boundaryCode() == 5 ||
      this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_supercriticalWeirCoefficient[index];
  return -9999.0;
}

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

double AdcircBoundary::pipeHeight(int index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeHeight[index];
  return -9999.0;
}

void AdcircBoundary::setPipeHeight(int index, double pipeHeight) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeHeight[index] = pipeHeight;
}

double AdcircBoundary::pipeDiameter(int index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeDiameter[index];
  return -9999.0;
}

void AdcircBoundary::setPipeDiameter(int index, double pipeDiameter) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeDiameter[index] = pipeDiameter;
}

double AdcircBoundary::pipeCoefficient(int index) const {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_pipeCoefficient[index];
  return -9999.0;
}

void AdcircBoundary::setPipeCoefficient(int index, double pipeCoefficient) {
  if (this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_pipeCoefficient[index] = pipeCoefficient;
}

AdcircNode *AdcircBoundary::node1(int index) const {
  if (index < this->boundaryLength() || index >= 0)
    return this->m_node1[index];
  return nullptr;
}

void AdcircBoundary::setNode1(int index, AdcircNode *node1) {
  if (index < this->boundaryLength() || index >= 0)
    this->m_node1[index] = node1;
}

AdcircNode *AdcircBoundary::node2(int index) const {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      return this->m_node2[index];
  return nullptr;
}

void AdcircBoundary::setNode2(int index, AdcircNode *node2) {
  if (this->boundaryCode() == 4 || this->boundaryCode() == 24 ||
      this->boundaryCode() == 5 || this->boundaryCode() == 25)
    if (index < this->boundaryLength() || index >= 0)
      this->m_node2[index] = node2;
}

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
          this->subcriticalWeirCoeffient(i) %
          this->supercriticalWeirCoefficient(i)));
    } else if (this->boundaryCode() == 5 || this->boundaryCode() == 25) {
      outputList.push_back(boost::str(
          boost::format("%11i %11i %6.3f %6.3f %6.3f %6.3f %6.3f %6.3f") %
          this->node1(i)->id() % this->node2(i)->id() %
          this->crestElevation(i) % this->subcriticalWeirCoeffient(i) %
          this->supercriticalWeirCoefficient(i) % this->pipeHeight(i) %
          this->pipeCoefficient(i) % this->pipeDiameter(i)));
    } else {
      outputList.push_back(
          boost::str(boost::format("%11i") % this->node1(i)->id()));
    }
  }
  return outputList;
}
