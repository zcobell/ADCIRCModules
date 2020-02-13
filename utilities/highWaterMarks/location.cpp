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
#include "location.h"
#include <cassert>
#include "fpcompare.h"

Location::Location() : m_x(0), m_y(0), m_measured(0), m_modeled(0) {}

double Location::x() const { return m_x; }

void Location::setX(double x) { m_x = x; }

double Location::y() const { return m_y; }

void Location::setY(double y) { m_y = y; }

double Location::measured() const { return m_measured; }

void Location::setMeasured(double measured) { m_measured = measured; }

double Location::modeled() const { return m_modeled; }

void Location::setModeled(double modeled) { m_modeled = modeled; }

double Location::difference() const {
  if (Adcirc::FpCompare::equalTo(this->m_modeled,
                                 Adcirc::Output::defaultOutputValue())) {
    return 0.0;
  } else {
    return this->m_modeled - this->m_measured;
  }
}

size_t Location::meshNode(const size_t &index) const {
  assert(index < 3);
  return this->m_weight.node_index[index];
}

void Location::setMeshNode(const size_t &index, const size_t &meshNode) {
  assert(index < 3);
  this->m_weight.node_index[index] = meshNode;
}

double Location::meshWeight(size_t index) const {
  assert(index < 3);
  return this->m_weight.weight[index];
}

void Location::setMeshWeight(const size_t index, double weight) {
  assert(index < 3);
  this->m_weight.weight[index] = weight;
}

Adcirc::Output::StationInterpolation::Weight *Location::weighting() {
  return &this->m_weight;
}

double Location::movedDist() const { return m_movedDist; }

void Location::setMovedDist(double movedDist) { m_movedDist = movedDist; }
