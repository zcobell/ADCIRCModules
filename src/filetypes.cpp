/*------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------*/
#include "filetypes.h"
#include <cassert>
#include <fstream>
#include <vector>
#include "error.h"
#include "fileio.h"
#include "netcdf.h"
#include "stringconversion.h"

namespace Adcirc {
namespace Output {
/**
 * @brief Check if the file type is ADCIRC ASCII Sparse
 * @param filename name of file to check format
 * @return true if the file format is ADCIRC ASCII Sparse
 */
bool checkFiletypeAsciiSparse(const std::string& filename) {
  assert(!filename.empty());

  std::fstream fid(filename);

  try {
    std::string line;
    std::getline(fid, line);  // header
    std::getline(fid, line);  // header
    std::getline(fid, line);  // first record

    std::vector<std::string> list;
    FileIO::Generic::splitString(line, list);

    if (list.size() == 4) {
      fid.close();
      return true;
    } else {
      fid.close();
      return false;
    }
  } catch (...) {
    if (fid.is_open()) {
      fid.close();
    }
    return false;
  }
}

/**
 * @brief Check if the file type is ADCIRC ASCII fill
 * @param filename name of file to check format
 * @return true if the file format is ADCIRC ASCII full
 */
bool checkFiletypeAsciiFull(const std::string& filename) {
  assert(!filename.empty());

  std::fstream fid(filename);

  try {
    std::string line;

    std::getline(fid, line);  // header
    std::getline(fid, line);  // header
    std::getline(fid, line);  // first record header

    std::vector<std::string> list;
    FileIO::Generic::splitString(line, list);

    if (list.size() == 2) {
      fid.close();
      return true;
    } else {
      fid.close();
      return false;
    }
  } catch (...) {
    if (fid.is_open()) {
      fid.close();
    }
    return false;
  }
}

/**
 * @brief Check if the file type is netCDF
 * @param filename name of file to check format
 * @return true if the file format is netCDF
 */
bool inquireNetcdfFormat(const std::string& filename, int& format) {
  int ncid;
  format = Adcirc::Output::OutputUnknown;
  int ierr = nc_open(filename.c_str(), NC_NOWRITE, &ncid);
  if (ierr != NC_NOERR) {
    return false;
  }
  ierr = nc_inq_format(ncid, &format);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    return false;
  }
  nc_close(ncid);
  return true;
}

/**
 * @brief Check if the file type is netcdf3
 * @param filename name of file to check format
 * @return true if the file format is netCDF3
 */
bool checkFiletypeNetcdf3(const std::string& filename) {
  int format;
  bool b = inquireNetcdfFormat(filename, format);
  if (b && format == NC_FORMAT_CLASSIC) {
    return true;
  }
  return false;
}

/**
 * @brief Check if the file type is netCDF4
 * @param filename name of file to check format
 * @return true if the file format is netCDF4
 */
bool checkFiletypeNetcdf4(const std::string& filename) {
  int format;
  bool b = inquireNetcdfFormat(filename, format);
  if (b) {
    if (format == NC_FORMAT_NETCDF4_CLASSIC || format == NC_FORMAT_NETCDF4) {
      return true;
    } else {
      return false;
    }
  } else {
    return false;
  }
}

/**
 * @brief Check if the file type is ADCIRC XDMF
 * @param filename name of file to check format
 * @return true if the file format is XDMF
 *
 * [NOT IMPLEMENTED]
 */
bool checkFiletypeXdmf(const std::string& filename) { return false; }

/**
 * @brief Check if the file type is ADCIRC ASCII harmonics
 * @param filename name of file to check format
 * @return true if the file format is ADCIRC ASCII Harmonics
 */
}  // namespace Output

namespace Harmonics {
bool checkFiletypeAsciiHarmonics(const std::string& filename) {
  std::fstream fid;
  fid.open(filename);
  if (!fid.is_open()) {
    return false;
  }

  std::string line;
  bool ok;

  std::getline(fid, line);
  size_t n = StringConversion::stringToSizet(line, ok);
  if (ok) {
  } else {
    fid.close();
    return false;
  }

  for (size_t i = 0; i < n; ++i) {
    std::vector<std::string> list;
    std::getline(fid, line);
    FileIO::Generic::splitString(line, list);
  }
  return true;
}

/**
 * @brief Check if the file type is ADCIRC netCDF harmonics
 * @param filename name of file to check format
 * @return true if the file format is ADCIRC netCDF Harmonics
 */
bool checkFiletypeNetcdfHarmonics(const std::string& filename) {
  int ncid;
  int ierr = nc_open(filename.c_str(), NC_NOWRITE, &ncid);
  if (ierr != NC_NOERR) {
    return false;
  }

  int dimid_nconst, varid_const;
  ierr = nc_inq_dimid(ncid, "num_const", &dimid_nconst);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    return false;
  }

  ierr = nc_inq_varid(ncid, "const", &varid_const);
  if (ierr != NC_NOERR) {
    nc_close(ncid);
    return false;
  }
  return true;
}
}  // namespace Harmonics
}  // namespace Adcirc
