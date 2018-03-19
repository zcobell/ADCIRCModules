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
#include "outputrecord.h"
#include <assert.h>
#include <math.h>

using namespace Adcirc::Output;

OutputRecord::OutputRecord(int numNodes, bool isVector) {
  assert(numNodes != 0);

  this->m_isVector = isVector;
  this->m_time = 0;
  this->m_iteration = 0;
  this->m_numNodes = numNodes;

  if (this->isVector()) {
    this->m_u.resize(this->numNodes());
    this->m_v.resize(this->numNodes());
  } else {
    this->m_u.resize(this->numNodes());
  }
}

long long OutputRecord::iteration() const { return this->m_iteration; }

void OutputRecord::setIteration(long long iteration) {
  this->m_iteration = iteration;
}

double OutputRecord::time() const { return this->m_time; }

void OutputRecord::setTime(double time) { this->m_time = time; }

int OutputRecord::numNodes() const { return this->m_numNodes; }

void OutputRecord::setNumNodes(int numNodes) { this->m_numNodes = numNodes; }

bool OutputRecord::isVector() const { return this->m_isVector; }

void OutputRecord::setIsVector(bool isVector) { this->m_isVector = isVector; }

void OutputRecord::setU(int index, double value) {
  assert(this->isVector());
  assert(index > 0 && index < this->numNodes());
  this->m_u[index] = value;
  return;
}

void OutputRecord::setV(int index, double value) {
  assert(this->isVector());
  assert(index > 0 && index < this->numNodes());
  this->m_v[index] = value;
  return;
}

void OutputRecord::set(int index, double value) {
  assert(!this->isVector());
  assert(index > 0 && index < this->numNodes());
  this->m_u[index] = value;
  return;
}

void OutputRecord::set(int index, double value_u, double value_v) {
  assert(this->isVector());
  assert(index > 0 && index < this->numNodes());
  this->m_u[index] = value_u;
  this->m_v[index] = value_v;
}

double OutputRecord::value(int index) {
  assert(index > 0 && index < this->numNodes());
  if (this->isVector())
    return pow(this->m_u[index], 2.0) + pow(this->m_v[index], 2.0);
  else
    return this->m_u[index];
}

double OutputRecord::direction(int index) {
  assert(this->isVector());
  assert(index > 0 && index < this->numNodes());
  return atan2(this->m_v[index], this->m_u[index]);
}

double OutputRecord::u(int index) {
  assert(this->isVector());
  assert(index > 0 && index < this->numNodes());
  return this->m_u[index];
}

double OutputRecord::v(int index) {
  assert(this->isVector());
  assert(index > 0 && index < this->numNodes());
  return this->m_v[index];
}
