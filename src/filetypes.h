#ifndef FILETYPES_H
#define FILETYPES_H

#include <string>

namespace Adcirc {
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
class Filetypes {
 public:
  Filetypes();
  static bool checkFiletypeAsciiFull(const std::string &filename);
  static bool checkFiletypeAsciiSparse(const std::string &filename);
  static bool checkFiletypeAsciiHarmonics(const std::string &filename);
  static bool checkFiletypeNetcdf3(const std::string &filename);
  static bool checkFiletypeNetcdf4(const std::string &filename);
  static bool checkFiletypeNetcdfHarmonics(const std::string &filename);
  static bool checkFiletypeXdmf(const std::string &filename);
  static bool inquireNetcdfFormat(const std::string &filename, int &format);
};

}  // namespace Output
}  // namespace Adcirc
#endif  // FILETYPES_H
