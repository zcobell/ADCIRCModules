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

/**
 * @class Filetypes
 * @author Zachary Cobell
 * @copyright Copyright 2018 Zachary Cobell. All Rights Reserved. This project
 * is released under the terms of the GNU General Public License v3
 * @brief The Filetypes class is used to determine the type of adcirc format
 * output file that is being read
 *
 */
class Filetypes {
 public:
  Filetypes() = default;
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
