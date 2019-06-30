#include "hash.h"
#include "hash_impl.h"

Hash::Hash(HashType h) : m_impl(new HashImpl(h)) {}

Hash::~Hash(){
    this->m_impl.reset(nullptr);
}

void Hash::addData(const std::string &s) { this->m_impl->addData(s); }

std::string Hash::getHash() { return this->m_impl->getHash(); }

HashType Hash::hashType() const { return this->m_impl->hashType(); }

void Hash::setHashType(const HashType &hashType) {
  this->m_impl->setHashType(hashType);
}
