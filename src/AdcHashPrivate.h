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
#ifndef ADCMOD_HASHPRIVATE_H
#define ADCMOD_HASHPRIVATE_H

#ifdef ADCMOD_HAVE_OPENSSL
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

#include <cstring>
#include <string>

#include "HashType.h"

namespace Adcirc {
namespace Private {
class HashPrivate {
 public:
  explicit HashPrivate(Adcirc::Cryptography::HashType h =
                           Adcirc::Cryptography::AdcircDefaultHash);
  void addData(const std::string &s);
  char *getHash();

  Adcirc::Cryptography::HashType hashType() const;
  void setHashType(const Adcirc::Cryptography::HashType &hashType);

 private:
  Adcirc::Cryptography::HashType m_hashType;
  bool m_started;

#ifdef ADCMOD_HAVE_OPENSSL
  void initialize();

  void addDataMd5(const std::string &data);
  void addDataSha1(const std::string &data);
  void addDataSha256(const std::string &data);

  char *getSha256();
  char *getSha1();
  char *getMd5();

  char *getDigest(size_t length, unsigned char data[]);

  void (HashPrivate::*addDataPtr)(const std::string &data);
  char *(HashPrivate::*getHashPtr)();

  MD5_CTX m_md5ctx;
  SHA_CTX m_sha1ctx;
  SHA256_CTX m_sha256ctx;
#endif
};
}  // namespace Private
}  // namespace Adcirc

#endif  // ADCMOD_HASHPRIVATE_H
