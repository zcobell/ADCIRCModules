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
#include "boundary.h"

#include <array>

#include "boost/format.hpp"
#include "default_values.h"
#include "hash.h"
#include "logging.h"

using namespace Adcirc::Geometry;

constexpr std::array<int, 7> c_bctypes_weir = {3, 13, 23, 4, 24, 5, 25};
constexpr std::array<int, 3> c_bctypes_externalWeir = {3, 13, 23};
constexpr std::array<int, 4> c_bctypes_internalWeir = {4, 24, 5, 25};
constexpr std::array<int, 2> c_bctypes_internalWeirWithoutPipes = {4, 24};
constexpr std::array<int, 2> c_bctypes_internalWeirWithPipes = {5, 25};
constexpr std::array<int, 15> c_bctypes_singleNodeBoundaries = {
    0, 1, 2, 10, 11, 12, 20, 21, 22, 30, 52, 30, 102, 112, 122};
constexpr std::array<int, 1> c_bctypes_openBoundaries = {-1};

/**
 * @brief Default constructor
 *
 * Initializes the boundary with code -1 and length 0.
 */
Boundary::Boundary()
    : m_boundaryCode(-1),
      m_averageLongitude(std::numeric_limits<double>::max()),
      m_hash(nullptr) {}

/**
 * @brief Initializes the boundary with user specified boundary type and length
 * @param[in] boundaryCode ADCIRC model boundary code
 * @param[in] boundaryLength number of nodes along this boundary
 */
Boundary::Boundary(int boundaryCode, size_t boundaryLength)
    : m_boundaryCode(boundaryCode),
      m_averageLongitude(std::numeric_limits<double>::max()),
      m_hash(nullptr) {
  this->setBoundaryLength(boundaryLength);
}

/**
 * @brief Function to copy data in a boundary. Used in the copy constructor and
 * copy assignment operators
 * @param[inout] a resultant boundary
 * @param[in] b boundary that is copied
 */
void Boundary::boundaryCopier(Boundary *a, const Boundary *b) {
  a->m_boundaryCode = b->boundaryCode();
  a->setBoundaryLength(b->boundaryLength());
  if (b->isOpenBoundary() || b->isSingleNodeBoundary()) {
    for (size_t i = 0; i < a->boundaryLength(); ++i) {
      a->m_node1[i] = b->node1(i);
    }
  } else if (b->isExternalWeir()) {
    for (size_t i = 0; i < a->boundaryLength(); ++i) {
      a->m_node1[i] = b->node1(i);
      a->m_crestElevation[i] = b->crestElevation(i);
      a->m_supercriticalWeirCoefficient[i] = b->supercriticalWeirCoefficient(i);
    }
  } else if (b->isInternalWeir()) {
    for (size_t i = 0; i < a->boundaryLength(); ++i) {
      a->m_node1[i] = b->node1(i);
      a->m_node2[i] = b->node2(i);
      a->m_crestElevation[i] = b->crestElevation(i);
      a->m_supercriticalWeirCoefficient[i] = b->supercriticalWeirCoefficient(i);
      a->m_subcriticalWeirCoefficient[i] = b->subcriticalWeirCoefficient(i);
    }
  } else if (b->isInternalWeirWithPipes()) {
    for (size_t i = 0; i < a->boundaryLength(); ++i) {
      a->m_node1[i] = b->node1(i);
      a->m_node2[i] = b->node2(i);
      a->m_crestElevation[i] = b->crestElevation(i);
      a->m_supercriticalWeirCoefficient[i] = b->supercriticalWeirCoefficient(i);
      a->m_subcriticalWeirCoefficient[i] = b->subcriticalWeirCoefficient(i);
      a->m_pipeHeight[i] = b->pipeHeight(i);
      a->m_pipeDiameter[i] = b->pipeDiameter(i);
      a->m_pipeCoefficient[i] = b->pipeCoefficient(i);
    }
  }
  a->m_hash.reset(nullptr);
}

/**
 * @brief Copy constructor
 * @param[in] b boundary to copy
 */
Boundary::Boundary(const Boundary &b) { Boundary::boundaryCopier(this, &b); }

/**
 * @brief Copy assignment operator
 * @param[in] b boundary to copy
 * @return pointer to copied boundary
 */
Boundary &Boundary::operator=(const Boundary &b) {
  Boundary::boundaryCopier(this, &b);
  return *this;
}

/**
 * @brief Returns true if this is an open boundary
 * @return true if open boundary
 */
bool Boundary::isOpenBoundary() const {
  return std::find(c_bctypes_openBoundaries.begin(),
                   c_bctypes_openBoundaries.end(),
                   this->m_boundaryCode) != c_bctypes_openBoundaries.end();
}

/**
 * @brief Returns true if boundary condition is some type of weir-type boundary
 * @return true if weir
 */
bool Boundary::isWeir() const {
  return std::find(c_bctypes_weir.begin(), c_bctypes_weir.end(),
                   this->m_boundaryCode) != c_bctypes_weir.end();
}

/**
 * @brief Returns true if boundary condition is an internal weir
 * @return true if internal weir
 */
bool Boundary::isInternalWeir() const {
  return std::find(c_bctypes_internalWeir.begin(), c_bctypes_internalWeir.end(),
                   this->m_boundaryCode) != c_bctypes_internalWeir.end();
}

/**
 * @brief Returns true if boundary condition is an external weir
 * @return true if external weir
 */
bool Boundary::isExternalWeir() const {
  return std::find(c_bctypes_externalWeir.begin(), c_bctypes_externalWeir.end(),
                   this->m_boundaryCode) != c_bctypes_externalWeir.end();
}

/**
 * @brief Returns true if boundary condition is an internal weir with cross
 * barrier pipes
 * @return true if weir with cross barrier pipes
 */
bool Boundary::isInternalWeirWithPipes() const {
  return std::find(c_bctypes_internalWeirWithPipes.begin(),
                   c_bctypes_internalWeirWithPipes.end(),
                   this->m_boundaryCode) !=
         c_bctypes_internalWeirWithPipes.end();
}

/**
 * @brief Returns true if boundary condition is internal weir without cross
 * barrier pipes
 * @return true if internal weir without cross barrier pipes
 */
bool Boundary::isInternalWeirWithoutPipes() const {
  return std::find(c_bctypes_internalWeirWithoutPipes.begin(),
                   c_bctypes_internalWeirWithoutPipes.end(),
                   this->m_boundaryCode) !=
         c_bctypes_internalWeirWithoutPipes.end();
}

/**
 * @brief Returns true if the boundary is some type of single node boundary
 * @return true if single node boundary
 */
bool Boundary::isSingleNodeBoundary() const {
  return std::find(c_bctypes_singleNodeBoundaries.begin(),
                   c_bctypes_singleNodeBoundaries.end(),
                   this->m_boundaryCode) !=
         c_bctypes_singleNodeBoundaries.end();
}

/**
 * @brief User specified initialization of the boundary
 * @param[in] boundaryCode ADCIRC model boundary code
 * @param[in] boundaryLength number of nodes along this boundary
 */
void Boundary::setBoundary(int boundaryCode, size_t boundaryLength) {
  this->setBoundaryCode(boundaryCode);
  this->setBoundaryLength(boundaryLength);
  this->m_averageLongitude = std::numeric_limits<double>::max();
}

/**
 * @brief Returns the length of the boundary
 * @return number of nodes in boundary
 */
size_t Boundary::boundaryLength() const { return this->m_node1.size(); }

/**
 * @brief Returns the length of the boundary. Same as
 * Boundary::bonudaryLength
 * @return number of nodes in boundary
 */
size_t Boundary::size() const { return this->boundaryLength(); }

/**
 * @brief Returns the length of the boundary. Same as
 * Boundary::bonudaryLength
 * @return number of nodes in boundary
 */
size_t Boundary::length() const { return this->boundaryLength(); }

/**
 * @brief Allocates the arrays used by the bounary
 * @param[in] boundaryLength number of nodes along the boundary
 */
void Boundary::setBoundaryLength(size_t boundaryLength) {
  if (this->boundaryLength() != boundaryLength) {
    this->m_node1.resize(boundaryLength);
    if (this->isWeir()) {
      this->m_crestElevation.resize(this->boundaryLength());
      this->m_supercriticalWeirCoefficient.resize(this->boundaryLength());
    }
    if (this->isInternalWeir()) {
      this->m_node2.resize(this->boundaryLength());
      this->m_subcriticalWeirCoefficient.resize(this->boundaryLength());
    }
    if (this->isInternalWeirWithPipes()) {
      this->m_pipeHeight.resize(this->boundaryLength());
      this->m_pipeDiameter.resize(this->boundaryLength());
      this->m_pipeCoefficient.resize(this->boundaryLength());
    }
  }
}

/**
 * @brief Returns the Adcirc model boundary code
 * @return Adcirc model boundary code
 */
int Boundary::boundaryCode() const { return this->m_boundaryCode; }

/**
 * @brief Sets the model boundary to the user specified code
 * @param[in] boundaryCode Adcirc model boundary code
 */
void Boundary::setBoundaryCode(int boundaryCode) {
  this->m_boundaryCode = boundaryCode;
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Returns the crest elevation for boundary types 3, 13, 23, 4, 24, 5,
 * and 25
 * @param[in] index position along the boundary
 * @return crest elevation if applicable, otherwise
 * adcircmodules_default_value<double>().
 */
double Boundary::crestElevation(size_t index) const {
  if (this->isWeir()) {
    if (index < this->boundaryLength()) {
      return this->m_crestElevation[index];
    }
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return adcircmodules_default_value<double>();
}

/**
 * @brief Sets the crest elevation for boundary types 3, 13, 23, 4, 24, 5, and
 * 25
 * @param[in] index position along the boundary
 * @param[in] crestElevation height above the datum for the weir crest
 */
void Boundary::setCrestElevation(size_t index, double crestElevation) {
  if (this->isWeir()) {
    if (index < this->boundaryLength()) {
      this->m_crestElevation[index] = crestElevation;
    } else {
      adcircmodules_throw_exception("Index exceeds bounds");
    }
  } else {
    adcircmodules_throw_exception("Invalid attribute for boundary type");
  }
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Returns the coeffieicnt of subcritical weir flow for boundary types 4,
 * 24, 5, and 25
 * @param[in] index position along the boundary
 * @return subcritical weir coefficient
 */
double Boundary::subcriticalWeirCoefficient(size_t index) const {
  if (this->isInternalWeir()) {
    if (index < this->boundaryLength()) {
      return this->m_subcriticalWeirCoefficient[index];
    }
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return adcircmodules_default_value<double>();
}

/**
 * @brief Sets the coefficient of subcritical flow for boundary types 4, 24, 5,
 * and 25
 * @param[in] index position along the boundary
 * @param[in] subcriticalWeirCoefficient coefficient of subcritical flow
 */
void Boundary::setSubcriticalWeirCoefficient(
    size_t index, double subcriticalWeirCoefficient) {
  if (this->isInternalWeir()) {
    if (index < this->boundaryLength()) {
      this->m_subcriticalWeirCoefficient[index] = subcriticalWeirCoefficient;
    } else {
      adcircmodules_throw_exception("Index exceeds bounds");
    }
  } else {
    adcircmodules_throw_exception("Invalid attribute for boundary type");
  }
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Returns the coefficient of supercritical flow for the specified
 * position along the boundary for boundary types 3, 13, 23, 4, 24, 5, and 25
 * @param[in] index position along the boundary
 * @return coefficient of supercritical flow
 */
double Boundary::supercriticalWeirCoefficient(size_t index) const {
  if (this->isWeir()) {
    if (index < this->boundaryLength()) {
      return this->m_supercriticalWeirCoefficient[index];
    }
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return adcircmodules_default_value<double>();
}

/**
 * @param[in] index position along the boundary
 * @param[in] supercriticalWeirCoefficient coefficient of supercritical flow for
 * boundarytypes 3, 13, 23, 4, 24, 5, and 25
 */
void Boundary::setSupercriticalWeirCoefficient(
    size_t index, double supercriticalWeirCoefficient) {
  if (this->isWeir()) {
    if (index < this->boundaryLength()) {
      this->m_supercriticalWeirCoefficient[index] =
          supercriticalWeirCoefficient;
    } else {
      adcircmodules_throw_exception("Index exceeds bounds");
    }
  } else {
    adcircmodules_throw_exception("Invalid attribute for boundary type");
  }
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Returns the elevation of the pipe above datum for type 5 and 25
 * boundaries
 * @param[in] index position along the boundary
 * @return height of pipe center above datum
 */
double Boundary::pipeHeight(size_t index) const {
  if (this->isInternalWeirWithPipes()) {
    if (index < this->boundaryLength()) {
      return this->m_pipeHeight[index];
    }
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return adcircmodules_default_value<double>();
}

/**
 * @brief Sets the elevation of the pipe above datum for type 5 and 25
 * boundaries
 * @param[in] index position along the boundary
 * @param[in] pipeHeight elevation of the pipe center above datum
 */
void Boundary::setPipeHeight(size_t index, double pipeHeight) {
  if (this->isInternalWeirWithPipes()) {
    if (index < this->boundaryLength()) {
      this->m_pipeHeight[index] = pipeHeight;
    } else {
      adcircmodules_throw_exception("Index exceeds bounds");
    }
  } else {
    adcircmodules_throw_exception("Invalid attribute for boundary type");
  }
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Returns the diameter of the pipe
 * @param[in] index position along the boundary
 * @return diameter of the pipe
 */
double Boundary::pipeDiameter(size_t index) const {
  if (this->isInternalWeirWithPipes()) {
    if (index < this->boundaryLength()) {
      return this->m_pipeDiameter[index];
    }
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return adcircmodules_default_value<double>();
}

/**
 * @brief Sets the diameter of the pipe
 * @param[in] index postion along the boundary
 * @param[in] pipeDiameter diameter of the pipe
 */
void Boundary::setPipeDiameter(size_t index, double pipeDiameter) {
  if (this->isInternalWeirWithPipes()) {
    if (index < this->boundaryLength()) {
      this->m_pipeDiameter[index] = pipeDiameter;
    } else {
      adcircmodules_throw_exception("Index exceeds bounds");
    }
  } else {
    adcircmodules_throw_exception("Index exceeds bounds");
  }
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Returns the pipe coefficient
 * @param[in] index position along the boundary
 * @return pipe coefficient
 */
double Boundary::pipeCoefficient(size_t index) const {
  if (this->isInternalWeirWithPipes()) {
    if (index < this->boundaryLength()) {
      return this->m_pipeCoefficient[index];
    }
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return adcircmodules_default_value<double>();
}

/**
 * @brief Sets the pipe coefficient
 * @param[in] index position along boundary
 * @param[in] pipeCoefficient pipe coefficient
 */
void Boundary::setPipeCoefficient(size_t index, double pipeCoefficient) {
  if (this->isInternalWeirWithPipes()) {
    if (index < this->boundaryLength()) {
      this->m_pipeCoefficient[index] = pipeCoefficient;
    } else {
      adcircmodules_throw_exception("Index exceeds bounds");
    }
  }
  if (this->m_hash != nullptr) this->generateHash();
}

/**
 * @brief Returns a pointer to the node on the boundary
 * @param[in] index position along the boundary
 * @return pointer to an Node object
 */
Node *Boundary::node1(size_t index) const {
  if (index < this->boundaryLength()) {
    return this->m_node1[index];
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return nullptr;
}

/**
 * @brief Sets a pointer to a node
 * @param[in] index position along the boundary
 * @param[in] node1 Pointer to an Node object
 */
void Boundary::setNode1(size_t index, Node *node1) {
  if (index < this->boundaryLength()) {
    this->m_node1[index] = node1;
  } else {
    adcircmodules_throw_exception("Index exceeds bounds");
  }
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Returns a pointer to the paired node for type 4, 24, 5, and 25
 * boundaries
 * @param[in] index position along the boundary
 * @return Node pointer
 */
Node *Boundary::node2(size_t index) const {
  if (this->isInternalWeir()) {
    if (index < this->boundaryLength()) {
      return this->m_node2[index];
    }
  }
  adcircmodules_throw_exception("Index exceeds bounds");
  return nullptr;
}

/**
 * @brief Sets the node pair for type 4, 24, 5, and 25 boundaries
 * @param[in] index position along the boundary
 * @param[in] node2 pointer to an Node object
 */
void Boundary::setNode2(size_t index, Node *node2) {
  if (this->isInternalWeir()) {
    if (index < this->boundaryLength()) {
      this->m_node2[index] = node2;
    } else {
      adcircmodules_throw_exception("Index exceeds bounds");
    }
  }
  if (this->m_hash.get() != nullptr) this->generateHash();
}

/**
 * @brief Writes the boundary into Adcirc ASCII mesh format and stores into a
 * string vector
 * @return vector of strings
 */
std::vector<std::string> Boundary::toStringList() {
  std::vector<std::string> outputList;
  outputList.reserve(this->size());

  if (this->isOpenBoundary()) {
    outputList.push_back(boost::str(boost::format("%11i") % this->length()));
  } else {
    outputList.push_back(boost::str(boost::format("%11i %11i") %
                                    this->length() % this->boundaryCode()));
  }

  for (size_t i = 0; i < this->length(); ++i) {
    if (this->isOpenBoundary() || this->isSingleNodeBoundary()) {
      outputList.push_back(
          boost::str(boost::format("%11i") % this->node1(i)->id()));
    } else if (this->isExternalWeir()) {
      outputList.push_back(boost::str(
          boost::format("%11i %6.3f %6.3f") % this->node1(i)->id() %
          this->crestElevation(i) % this->supercriticalWeirCoefficient(i)));
    } else if (this->isInternalWeirWithoutPipes()) {
      outputList.push_back(boost::str(
          boost::format("%11i %11i %6.3f %6.3f %6.3f") % this->node1(i)->id() %
          this->node2(i)->id() % this->crestElevation(i) %
          this->subcriticalWeirCoefficient(i) %
          this->supercriticalWeirCoefficient(i)));
    } else if (this->isInternalWeirWithPipes()) {
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

/**
 * @brief Adds a single node type boundary node to the boundary string
 * @param[in] node pointer to node to add to boundary
 */
void Boundary::addNode(Node *node) {
  assert(this->isSingleNodeBoundary());
  if (!this->isSingleNodeBoundary()) {
    adcircmodules_throw_exception(
        "Incorrect addNode call for this boundary type");
  }
  this->m_node1.push_back(node);
  return;
}

/**
 * @brief Adds an external weir type boundary node to the boundary string
 * @param[in] node pointer to node
 * @param[in] crestElevation crest elevation of external weir
 * @param[in] supercriticalWeirCoefficient coefficient for external weir
 */
void Boundary::addNode(Node *node, double crestElevation,
                       double supercriticalWeirCoefficient) {
  assert(this->isExternalWeir());
  if (!this->isExternalWeir()) {
    adcircmodules_throw_exception(
        "Incorrect addNode call for this boundary type");
  }
  this->m_node1.push_back(node);
  this->m_crestElevation.push_back(crestElevation);
  this->m_supercriticalWeirCoefficient.push_back(supercriticalWeirCoefficient);
}

/**
 * @brief Adds an internal weir type boundary node to the boundary string
 * @param[in] node1 pointer to node 1
 * @param[in] node2 pointer to node 2
 * @param[in] crestElevation crest elevation of internal weir
 * @param[in] supercriticalWeirCoefficient coefficient of internal weir
 * supercritical flow
 * @param[in] subcriticalWeirCoefficient coefficient of internal weir
 * subcritical flow
 */
void Boundary::addNode(Node *node1, Node *node2, double crestElevation,
                       double supercriticalWeirCoefficient,
                       double subcriticalWeirCoefficient) {
  assert(this->isInternalWeirWithoutPipes());
  if (!this->isInternalWeirWithoutPipes()) {
    adcircmodules_throw_exception(
        "Incorrect addNode call for this boundary type");
  }
  this->m_node1.push_back(node1);
  this->m_node2.push_back(node2);
  this->m_crestElevation.push_back(crestElevation);
  this->m_subcriticalWeirCoefficient.push_back(subcriticalWeirCoefficient);
  this->m_supercriticalWeirCoefficient.push_back(supercriticalWeirCoefficient);
  return;
}

/**
 * @brief Adds an internal weir type boudnary with cross-barrier pipes to the
 * boundary string
 * @param[in] node1 pointer to node 1
 * @param[in] node2 pointer to node 2
 * @param[in] crestElevation crest elevation of internal weir
 * @param[in] supercriticalWeirCoefficient coefficient of internal weir
 * supercritical flow
 * @param[in] pipeHeight elevation of pipe
 * @param[in] pipeDiameter diameter of pipe
 * @param[in] pipeCoefficient coefficient of pipe
 */
void Boundary::addNode(Node *node1, Node *node2, double crestElevation,
                       double supercriticalWeirCoefficient,
                       double subcriticalWeirCoefficient, double pipeHeight,
                       double pipeDiameter, double pipeCoefficient) {
  assert(this->isInternalWeirWithPipes());
  if (!this->isInternalWeirWithPipes()) {
    adcircmodules_throw_exception(
        "Incorrect addNode call for this boundary type");
  }
  this->m_node1.push_back(node1);
  this->m_node2.push_back(node2);
  this->m_crestElevation.push_back(crestElevation);
  this->m_subcriticalWeirCoefficient.push_back(subcriticalWeirCoefficient);
  this->m_supercriticalWeirCoefficient.push_back(supercriticalWeirCoefficient);
  this->m_pipeDiameter.push_back(pipeDiameter);
  this->m_pipeHeight.push_back(pipeHeight);
  this->m_pipeCoefficient.push_back(pipeCoefficient);
  return;
}

/**
 * @brief Returns the hash of the boundary based upon boundary type nodes,
 * heights, and coefficients
 * @return hash formatted as string
 */
std::string Boundary::hash(Adcirc::Cryptography::HashType h, bool force) {
  if (this->m_hash.get() == nullptr || force) this->generateHash(h);
  return std::string(this->m_hash.get());
}

/**
 * @brief Generates the hash data for this boundary
 * @param[in] h cryptographic hashing algorithm to use
 */
void Boundary::generateHash(Adcirc::Cryptography::HashType h) {
  Adcirc::Cryptography::Hash hash(h);
  for (size_t i = 0; i < this->boundaryLength(); ++i) {
    hash.addData(boost::str(boost::format("%3.3i") % this->m_boundaryCode));
    hash.addData(this->m_node1[i]->positionHash());
    if (this->isWeir()) {
      if (this->isInternalWeir())
        hash.addData(this->m_node2[i]->positionHash());
      hash.addData(
          boost::str(boost::format("%6.3f") % this->m_crestElevation[i]));
      hash.addData(boost::str(boost::format("%6.3f") %
                              this->m_supercriticalWeirCoefficient[i]));
      if (this->isInternalWeir()) {
        hash.addData(boost::str(boost::format("%6.3f") %
                                this->m_subcriticalWeirCoefficient[i]));
        if (this->isInternalWeirWithPipes()) {
          hash.addData(
              boost::str(boost::format("%6.3f") % this->m_pipeDiameter[i]));
          hash.addData(
              boost::str(boost::format("%6.3f") % this->m_pipeHeight[i]));
          hash.addData(
              boost::str(boost::format("%6.3f") % this->m_pipeCoefficient[i]));
        }
      }
    }
  }
  this->m_hash.reset(hash.getHash());
  return;
}

/**
 * @brief Returns the average longitude of the boundary
 * @param[in] force force recalculation of the average longitude
 * @return average longitude
 */
double Boundary::averageLongitude(bool force) {
  if (force || this->m_averageLongitude == std::numeric_limits<double>::max()) {
    this->calculateAverageLongitude();
  }
  return this->m_averageLongitude;
}

/**
 * @brief Calculates the average longitude along the boundary
 */
void Boundary::calculateAverageLongitude() {
  double position = 0.0;
  for (size_t i = 0; i < this->size(); ++i) {
    double x;
    if (this->isInternalWeir()) {
      x = (this->m_node1[i]->x() + this->m_node2[i]->x()) / 2.0;
    } else {
      x = this->m_node1[i]->x();
    }
    position += x;
  }
  this->m_averageLongitude = position / this->size();
  return;
}
