//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#include <iostream>
#include "adcircmodules.h"

int main(int argc, char *argv[]) {
  std::string hashData = "zach";
  Hash md5(HashType::AdcmodMD5);;
  Hash sha1(HashType::AdcmodSHA1);;
  Hash sha256(HashType::AdcmodSHA256);

  md5.addData(hashData);
  sha1.addData(hashData);
  sha256.addData(hashData);

  char* char_hashed_md5 = md5.getHash();
  char* char_hashed_sha1 = sha1.getHash();
  char* char_hashed_sha256 = sha256.getHash();

  std::string hashed_md5(char_hashed_md5);
  std::string hashed_sha1(char_hashed_sha1);
  std::string hashed_sha256(char_hashed_sha256);

  delete[] char_hashed_md5;
  delete[] char_hashed_sha1;
  delete[] char_hashed_sha256;

  if(hashed_md5 == "d61616c3e2a6cf59a0cc435a66c091d4" &&
     hashed_sha1 == "9de8c4480303b5335cd2a33eefe814615ba3612a" &&
     hashed_sha256 == "cb9b5a0f4a8b09ba490e3acc902f38acf85205797cd59654645e5c1ef8c1ada0" ){
    return 0;
  } else {
    return 1;
  }
}
