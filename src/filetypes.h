#ifndef FILETYPES_H
#define FILETYPES_H

#include <string>

namespace Adcirc {

namespace Geometry {
enum MeshFormat {
  /// Unknown mesh format
  MeshUnknown = 0x201,
  /// ADCIRC mesh format (*.14, *.grd)
  MeshAdcirc = 0x202,
  /// ADCIRC mesh in netcdf format (*.nc)
  MeshAdcircNetcdf = 0x203,
  /// Aquaveo generic mesh format (*.2dm)
  Mesh2DM = 0x204,
  /// Deltares D-Flow FM format (*_net.nc)
  MeshDFlow = 0x205
};
}

namespace Harmonics {
enum HarmonicsFormat {
  /// Ascii formatted harmonics files (fort.53, fort.54)
  HarmonicsAscii = 0x101,
  /// NetCDF formatted harmonics files (fort.53.nc, fort.54.nc)
  HarmonicsNetcdf = 0x102,
  /// Unknown harmonics file format
  HarmonicsUnknown = 0x999
};
bool checkFiletypeAsciiHarmonics(const std::string &filename);
bool checkFiletypeNetcdfHarmonics(const std::string &filename);
}  // namespace Harmonics

namespace Output {

enum OutputFormat {
  /// Full ascii formatted Adcirc output
  OutputAsciiFull = 0x001,
  /// Sparse formatted Adcirc output
  OutputAsciiSparse = 0x002,
  /// netCDF3 formatted Adcirc output
  OutputNetcdf3 = 0x003,
  /// netCDF4 formatted Adcirc output
  OutputNetcdf4 = 0x004,
  /// Xdmf formatted Adcirc output [not implemented]
  OutputXdmf = 0x005,
  /// Unknown output format
  OutputUnknown = 0x999
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
