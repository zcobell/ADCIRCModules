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
#ifndef ADCMOD_HASH_H
#define ADCMOD_HASH_H

#include <memory>
#include <string>
#include "hashtype.h"

namespace Adcirc {

namespace Private {
// Forward declaration of pimpl class
class HashPrivate;
}  // namespace Private

namespace Cryptography {

/**
 * @class Hash
 * @author Zachary Cobell
 * @brief Class that handles generation of OpenSSL based cryptographic hashes
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 *
 * The Hash class generates cryptographic hashes using the OpenSSL library. The
 * class can generate md5, sha1, and sha256 based hashes. The default hash is
 * Sha1. The use for this class is to generate unique indentifiers for Adcirc
 * objects
 *
 */
class Hash {
 public:
  Hash(Adcirc::Cryptography::HashType h = Adcirc::Cryptography::AdcircDefaultHash);
  ~Hash();
  void addData(const std::string &s);
  char *getHash();

  Adcirc::Cryptography::HashType hashType() const;
  void setHashType(const Adcirc::Cryptography::HashType &hashType);

 private:
  std::unique_ptr<Adcirc::Private::HashPrivate> m_impl;
};
}  // namespace Cryptography
}  // namespace Adcirc

#endif  // ADCMOD_HASH_H
