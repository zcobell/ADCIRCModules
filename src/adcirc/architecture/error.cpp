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
#include "error.h"
#include <unordered_map>

using namespace std;

static unordered_map<int, string> m_errorCodes = {
    make_pair(Adcirc::NoError, "No Error"),
    make_pair(Adcirc::UnknownError, "Unknown Error"),
    make_pair(Adcirc::MissingFile, "Missing File"),
    make_pair(Adcirc::FileOpenError, "Error opening file"),
    make_pair(Adcirc::FileReadError, "Generic error during file read"),
    make_pair(Adcirc::FileCloseError, "Error closing file"),
    make_pair(Adcirc::FileNotOpenError,
              "File not opened when access attempted"),
    make_pair(Adcirc::Proj4Error, "Error during use of Proj4 library"),
    make_pair(Adcirc::KdtreeError, "Error during use of the KDTree2 library"),
    make_pair(Adcirc::Geometry::InvalidFormat, "Invalid format detected"),
    make_pair(Adcirc::ModelParameters::MeshMismatch,
              "Mesh does not match the paramteters file"),
    make_pair(Adcirc::Output::OutputReadError,
              "Error during read of ADCIRC output"),
    make_pair(Adcirc::Output::NetcdfVarNotFound,
              "No valid ADCIRC netcdf variable found in file"),
    make_pair(Adcirc::Output::NetcdfTooManyVariablesFound,
              "More than 2 netcdf variables found in netcdf output file")};

string Adcirc::errorString(int code) {
  if (m_errorCodes.find(code) != m_errorCodes.end())
    return m_errorCodes[code];
  else
    return m_errorCodes[Adcirc::UnknownError];
}
