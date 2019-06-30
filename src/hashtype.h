#ifndef HASHTYPE_H
#define HASHTYPE_H

enum HashType { AdcmodMD5, AdcmodSHA1, AdcmodSHA256 };

constexpr HashType AdcircDefaultHash = HashType::AdcmodMD5;

#endif // HASHTYPE_H
