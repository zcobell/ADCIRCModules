#ifndef HASH_H
#define HASH_H

#include <memory>
#include "hashtype.h"

class HashImpl;

class Hash {
 public:
  Hash(HashType h = AdcircDefaultHash);
  ~Hash();
  void addData(const std::string &s);
  std::string getHash();

  HashType hashType() const;
  void setHashType(const HashType &hashType);

 private:
  std::unique_ptr<HashImpl> m_impl;
};

#endif  // HASH_H
