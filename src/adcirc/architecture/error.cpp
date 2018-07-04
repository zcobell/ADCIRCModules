#include "error.h"
#include <unordered_map>

using namespace std;

static unordered_map<int, string> m_errorCodes = {
    std::make_pair(Adcirc::NoError, "No Error"),
    std::make_pair(Adcirc::UnknownError, "Unknown Error"),
    std::make_pair(Adcirc::MissingFile, "Missing File"),
    std::make_pair(Adcirc::FileOpenError, "Error opening file"),
    std::make_pair(Adcirc::FileReadError, "Generic error during file read"),
    std::make_pair(Adcirc::FileCloseError, "Error closing file"),
    std::make_pair(Adcirc::FileNotOpenError,
                   "File not opened when access attempted"),
    std::make_pair(Adcirc::Proj4Error, "Error during use of Proj4 library"),
    std::make_pair(Adcirc::KdtreeError,
                   "Error during use of the KDTree2 library"),
    std::make_pair(Adcirc::Geometry::InvalidFormat, "Invalid format detected"),
    std::make_pair(Adcirc::ModelParameters::MeshMismatch,
                   "Mesh does not match the paramteters file"),
    std::make_pair(Adcirc::Output::OutputReadError,
                   "Error during read of ADCIRC output"),
    std::make_pair(Adcirc::Output::NetcdfVarNotFound,
                   "No valid ADCIRC netcdf variable found in file"),
    std::make_pair(Adcirc::Output::NetcdfTooManyVariablesFound,
                   "More than 2 netcdf variables found in netcdf output file")};

string Adcirc::errorString(int code) {
  if (m_errorCodes.find(code) != m_errorCodes.end())
    return m_errorCodes[code];
  else
    return m_errorCodes[Adcirc::UnknownError];
}
