#ifndef FILETYPES_H
#define FILETYPES_H

#include <string>

namespace Adcirc {

namespace Harmonics {
enum HarmonicsFormat { ASCII, NETCDF4 };
bool checkFiletypeAsciiHarmonics(const std::string &filename);
bool checkFiletypeNetcdfHarmonics(const std::string &filename);
}  // namespace Harmonics

namespace Output {

enum _fileTypes {
  ASCIIFull,
  ASCIISparse,
  ASCIIHarmonics,
  Netcdf3,
  Netcdf4,
  Xdmf,
  Unknown
};

bool checkFiletypeAsciiFull(const std::string &filename);
bool checkFiletypeAsciiSparse(const std::string &filename);
bool checkFiletypeNetcdf3(const std::string &filename);
bool checkFiletypeNetcdf4(const std::string &filename);
bool checkFiletypeXdmf(const std::string &filename);
bool inquireNetcdfFormat(const std::string &filename, int &format);

}  // namespace Output
}  // namespace Adcirc
#endif  // FILETYPES_H
