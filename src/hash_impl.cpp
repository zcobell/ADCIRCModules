#include "hash_impl.h"

HashImpl::HashImpl(HashType h) : m_hashType(h) {
  MD5_Init(&this->m_md5ctx);
  SHA1_Init(&this->m_sha1ctx);
  SHA256_Init(&this->m_sha256ctx);
}

void HashImpl::addData(const std::string &s) {
  switch (this->m_hashType) {
    case HashType::AdcmodMD5:
      this->addDataMd5(s);
      break;
    case HashType::AdcmodSHA1:
      this->addDataSha1(s);
      break;
    case HashType::AdcmodSHA256:
      this->addDataSha256(s);
      break;
  }
  return;
}

std::string HashImpl::getHash() {
  switch (this->m_hashType) {
    case HashType::AdcmodMD5:
      return this->getMd5();
    case HashType::AdcmodSHA1:
      return this->getSha1();
    case HashType::AdcmodSHA256:
      return this->getSha256();
  }
}

void HashImpl::addDataMd5(const std::string &data) {
  const char *string = data.c_str();
  MD5_Update(&this->m_md5ctx, string, strlen(string));
  return;
}

void HashImpl::addDataSha1(const std::string &data) {
  const char *string = data.c_str();
  SHA1_Update(&this->m_sha1ctx, string, strlen(string));
  return;
}

void HashImpl::addDataSha256(const std::string &data) {
  const char *string = data.c_str();
  SHA256_Update(&this->m_sha256ctx, string, strlen(string));
  return;
}

std::string HashImpl::getDigest(size_t length, unsigned char data[]) {
  char *mdString = new char[length * 2 + 1];
  for (size_t i = 0; i < length; ++i)
    sprintf(&mdString[i * 2], "%02x", (unsigned int)data[i]);
  return std::string(mdString);
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

HashType HashImpl::hashType() const { return this->m_hashType; }

void HashImpl::setHashType(const HashType &hashType) {
  this->m_hashType = hashType;
}
