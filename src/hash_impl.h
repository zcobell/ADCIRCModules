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
#ifndef ADCMOD_HASHIMPL_H
#define ADCMOD_HASHIMPL_H

#ifdef ADCMOD_HAVE_OPENSSL
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

#include <cstring>
#include <string>
#include "hashtype.h"

class HashImpl {
 public:
  HashImpl(HashType h = AdcircDefaultHash);
  void addData(const std::string &s);
  std::string getHash();

  HashType hashType() const;
  void setHashType(const HashType &hashType);

 private:
  HashType m_hashType;
  bool m_started;

#ifdef ADCMOD_HAVE_OPENSSL
  void initialize();

  void addDataMd5(const std::string &data);
  void addDataSha1(const std::string &data);
  void addDataSha256(const std::string &data);

  std::string getSha256();
  std::string getSha1();
  std::string getMd5();

  std::string getDigest(size_t length, unsigned char data[]);

  void (HashImpl::*addDataPtr)(const std::string &data);
  std::string (HashImpl::*getHashPtr)();

  MD5_CTX m_md5ctx;
  SHA_CTX m_sha1ctx;
  SHA256_CTX m_sha256ctx;
#endif
};

#endif  // ADCMOD_HASHIMPL_H
