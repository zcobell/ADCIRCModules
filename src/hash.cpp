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
#include "hash.h"
#include <string>
#include "hash_private.h"

using namespace Adcirc::Cryptography;

Hash::Hash(Adcirc::Cryptography::HashType h) : m_impl(new Adcirc::Private::HashPrivate(h)) {}

Hash::~Hash() { this->m_impl.reset(nullptr); }

void Hash::addData(const std::string &s) { this->m_impl->addData(s); }

char *Hash::getHash() { return this->m_impl->getHash(); }

Adcirc::Cryptography::HashType Hash::hashType() const { return this->m_impl->hashType(); }

void Hash::setHashType(const Adcirc::Cryptography::HashType &hashType) {
  this->m_impl->setHashType(hashType);
}
