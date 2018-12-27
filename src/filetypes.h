#ifndef FILETYPES_H
#define FILETYPES_H

#include <string>

namespace Adcirc {

namespace Harmonics {
enum HarmonicsFormat { ASCII = 0x101, NETCDF4 = 0x102, Unknown = 0x999 };
bool checkFiletypeAsciiHarmonics(const std::string &filename);
bool checkFiletypeNetcdfHarmonics(const std::string &filename);
}  // namespace Harmonics

namespace Output {

enum _fileTypes {
  ASCIIFull = 0x001,
  ASCIISparse = 0x002,
  ASCIIHarmonics = 0x003,
  Netcdf3 = 0x004,
  Netcdf4 = 0x005,
  Xdmf = 0x006,
  Unknown = 0x999
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
