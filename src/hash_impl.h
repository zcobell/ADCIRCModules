#ifndef ADCMOD_HASHIMPL_H
#define ADCMOD_HASHIMPL_H

#include <openssl/md5.h>
#include <openssl/sha.h>
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
  void initialize();

  void addDataMd5(const std::string &data);
  void addDataSha1(const std::string &data);
  void addDataSha256(const std::string &data);

  std::string getSha256();
  std::string getSha1();
  std::string getMd5();

  std::string getDigest(size_t length, unsigned char data[]);

  bool m_started;

  void (HashImpl::*addDataPtr)(const std::string &data);
  std::string (HashImpl::*getHashPtr)();

  HashType m_hashType;
  MD5_CTX m_md5ctx;
  SHA_CTX m_sha1ctx;
  SHA256_CTX m_sha256ctx;
};

#endif  // ADCMOD_HASHIMPL_H
