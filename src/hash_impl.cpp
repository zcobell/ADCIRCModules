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
#include "hash_impl.h"
#include "logging.h"

HashImpl::HashImpl(HashType h) : m_hashType(h), m_started(false) {}

HashType HashImpl::hashType() const { return this->m_hashType; }

void HashImpl::setHashType(const HashType &hashType) {
  if (this->m_started) {
    Adcirc::Logging::warning(
        "Cannot change hash type once data has been added");
  } else {
    this->m_hashType = hashType;
  }
  return;
}

#ifndef ADCMOD_HAVE_OPENSSL
void HashImpl::addData(const std::string &s) { return; }

std::string HashImpl::getHash() { return std::string(); }

#else
void HashImpl::initialize() {
  this->m_started = true;
  switch (this->m_hashType) {
    case HashType::AdcmodMD5:
      MD5_Init(&this->m_md5ctx);
      this->addDataPtr = &HashImpl::addDataMd5;
      this->getHashPtr = &HashImpl::getMd5;
      break;
    case HashType::AdcmodSHA1:
      SHA1_Init(&this->m_sha1ctx);
      this->addDataPtr = &HashImpl::addDataSha1;
      this->getHashPtr = &HashImpl::getSha1;
      break;
    case HashType::AdcmodSHA256:
      SHA256_Init(&this->m_sha256ctx);
      this->addDataPtr = &HashImpl::addDataSha256;
      this->getHashPtr = &HashImpl::getSha256;
      break;
    default:
      break;
  }
  return;
}

void HashImpl::addData(const std::string &s) {
  if (!this->m_started) this->initialize();
  (this->*addDataPtr)(s);
  return;
}

std::string HashImpl::getHash() { return (this->*getHashPtr)(); }

void HashImpl::addDataMd5(const std::string &data) {
  MD5_Update(&this->m_md5ctx, &data[0], data.length());
  return;
}

void HashImpl::addDataSha1(const std::string &data) {
  SHA1_Update(&this->m_sha1ctx, &data[0], data.length());
  return;
}

void HashImpl::addDataSha256(const std::string &data) {
  SHA256_Update(&this->m_sha256ctx, &data[0], data.length());
  return;
}

std::string HashImpl::getDigest(size_t length, unsigned char data[]) {
  char *mdString = new char[length * 2 + 1];
  for (size_t i = 0; i < length; ++i)
    sprintf(&mdString[i * 2], "%02x", static_cast<unsigned int>(data[i]));
  std::string output(mdString);
  delete[] mdString;
  return output;
}

std::string HashImpl::getMd5() {
  unsigned char digest[MD5_DIGEST_LENGTH];
  MD5_Final(digest, &this->m_md5ctx);
  return this->getDigest(MD5_DIGEST_LENGTH, digest);
}

std::string HashImpl::getSha1() {
  unsigned char digest[SHA_DIGEST_LENGTH];
  SHA1_Final(digest, &this->m_sha1ctx);
  return this->getDigest(SHA_DIGEST_LENGTH, digest);
}

std::string HashImpl::getSha256() {
  unsigned char digest[SHA256_DIGEST_LENGTH];
  SHA256_Final(digest, &this->m_sha256ctx);
  return this->getDigest(SHA256_DIGEST_LENGTH, digest);
}
#endif
