#include "filetypes.h"
#include <cassert>
#include <fstream>
#include <vector>
#include "error.h"
#include "io.h"
#include "netcdf.h"
#include "stringconversion.h"

using namespace Adcirc::Output;

bool Filetypes::checkFiletypeAsciiSparse(const std::string& filename) {
  assert(!filename.empty());

  std::fstream fid(filename);

  try {
    std::string line;
    std::getline(fid, line);  // header
    std::getline(fid, line);  // header
    std::getline(fid, line);  // first record

    std::vector<std::string> list;
    int ierr = IO::splitString(line, list);
    if (ierr != 0) {
      fid.close();
      return false;
    }

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

bool Filetypes::checkFiletypeAsciiFull(const std::string& filename) {
  assert(!filename.empty());

  std::fstream fid(filename);

  try {
    std::string line;

    std::getline(fid, line);  // header
    std::getline(fid, line);  // header
    std::getline(fid, line);  // first record header

    std::vector<std::string> list;
    int ierr = IO::splitString(line, list);
    if (ierr != 0) {
      fid.close();
      return false;
    }

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

bool Filetypes::inquireNetcdfFormat(const std::string& filename, int& format) {
  int ncid;
  format = Adcirc::Output::Unknown;
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

bool Filetypes::checkFiletypeNetcdf3(const std::string &filename) {
  int format;
  bool b = Filetypes::inquireNetcdfFormat(filename, format);
  if (b && format == NC_FORMAT_CLASSIC) {
    return true;
  }
  return false;
}

bool Filetypes::checkFiletypeNetcdf4(const std::string &filename) {
  int format;
  bool b = Filetypes::inquireNetcdfFormat(filename, format);
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

bool Filetypes::checkFiletypeXdmf(const std::string& filename) { return false; }

bool Filetypes::checkFiletypeAsciiHarmonics(const std::string& filename) {
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
    int ierr = IO::splitString(line, list);
    if (ierr != Adcirc::NoError) {
      return false;
    }
  }
  return true;
}

bool Filetypes::checkFiletypeNetcdfHarmonics(const std::string &filename) {
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
