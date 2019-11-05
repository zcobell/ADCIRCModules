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
#include "hash_private.h"

#include "logging.h"

using namespace Adcirc::Private;

HashPrivate::HashPrivate(Adcirc::Cryptography::HashType h)
    : m_hashType(h), m_started(false) {
#ifdef ADCMOD_HAVE_OPENSSL
  this->addDataPtr = nullptr;
  this->getHashPtr = nullptr;
#endif
}

Adcirc::Cryptography::HashType HashPrivate::hashType() const {
  return this->m_hashType;
}

void HashPrivate::setHashType(const Adcirc::Cryptography::HashType &hashType) {
  if (this->m_started) {
    Adcirc::Logging::warning(
        "Cannot change hash type once data has been added");
  } else {
    this->m_hashType = hashType;
  }
  return;
}

#ifndef ADCMOD_HAVE_OPENSSL
void HashPrivate::addData(const std::string &s) {
  adcircmodules_throw_exception("OpenSSL library not enabled.");
  return;
}

char *HashPrivate::getHash() {
  adcircmodules_throw_exception("OpenSSL library not enabled.");
  return nullptr;
}

#else
void HashPrivate::initialize() {
  this->m_started = true;
  switch (this->m_hashType) {
    case Adcirc::Cryptography::HashType::AdcmodMD5:
      MD5_Init(&this->m_md5ctx);
      this->addDataPtr = &HashPrivate::addDataMd5;
      this->getHashPtr = &HashPrivate::getMd5;
      break;
    case Adcirc::Cryptography::HashType::AdcmodSHA1:
      SHA1_Init(&this->m_sha1ctx);
      this->addDataPtr = &HashPrivate::addDataSha1;
      this->getHashPtr = &HashPrivate::getSha1;
      break;
    case Adcirc::Cryptography::HashType::AdcmodSHA256:
      SHA256_Init(&this->m_sha256ctx);
      this->addDataPtr = &HashPrivate::addDataSha256;
      this->getHashPtr = &HashPrivate::getSha256;
      break;
    default:
      break;
  }
  return;
}

void HashPrivate::addData(const std::string &s) {
  if (!this->m_started) this->initialize();
  (this->*addDataPtr)(s);
  return;
}

char *HashPrivate::getHash() { return (this->*getHashPtr)(); }

void HashPrivate::addDataMd5(const std::string &data) {
  MD5_Update(&this->m_md5ctx, &data[0], data.length());
  return;
}

void HashPrivate::addDataSha1(const std::string &data) {
  SHA1_Update(&this->m_sha1ctx, &data[0], data.length());
  return;
}

void HashPrivate::addDataSha256(const std::string &data) {
  SHA256_Update(&this->m_sha256ctx, &data[0], data.length());
  return;
}

char *HashPrivate::getDigest(size_t length, unsigned char data[]) {
  char *mdString = new char[length * 2 + 1];
  for (size_t i = 0; i < length; ++i)
    sprintf(&mdString[i * 2], "%02x", static_cast<unsigned int>(data[i]));
  return mdString;
}

char *HashPrivate::getMd5() {
  unsigned char digest[MD5_DIGEST_LENGTH];
  MD5_Final(digest, &this->m_md5ctx);
  return this->getDigest(MD5_DIGEST_LENGTH, digest);
}

char *HashPrivate::getSha1() {
  unsigned char digest[SHA_DIGEST_LENGTH];
  SHA1_Final(digest, &this->m_sha1ctx);
  return this->getDigest(SHA_DIGEST_LENGTH, digest);
}

char *HashPrivate::getSha256() {
  unsigned char digest[SHA256_DIGEST_LENGTH];
  SHA256_Final(digest, &this->m_sha256ctx);
  return this->getDigest(SHA256_DIGEST_LENGTH, digest);
}
#endif
