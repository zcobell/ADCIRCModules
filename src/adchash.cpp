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
#include "adchash.h"
#include <string>
#include "adchash_private.h"

using namespace Adcirc::Cryptography;

/**
 * @brief Constructor for hash object
 * @param[in] h cryptographic hash type to use
 */
Hash::Hash(Adcirc::Cryptography::HashType h) : m_impl(new Adcirc::Private::HashPrivate(h)) {}

/**
 * @brief Destructor
 */
Hash::~Hash() { this->m_impl.reset(nullptr); }

/**
 * @brief Adds data to the hash
 * @param[in] s string data to add to hash
 */
void Hash::addData(const std::string &s) { this->m_impl->addData(s); }

/**
 * @brief Returns a char pointer to the hash
 * @return char pointer with hash data
 */
char *Hash::getHash() { return this->m_impl->getHash(); }

/**
 * @brief Returns the hash type currently being used
 * @return hash type
 */
Adcirc::Cryptography::HashType Hash::hashType() const { return this->m_impl->hashType(); }

/**
 * @brief Sets the hash type to use
 * @param[in] hashType type of cryptographic hash
 */
void Hash::setHashType(const Adcirc::Cryptography::HashType &hashType) {
  this->m_impl->setHashType(hashType);
}
