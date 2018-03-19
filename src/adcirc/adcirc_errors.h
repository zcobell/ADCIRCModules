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
#ifndef ADCIRCERRORS_H
#define ADCIRCERRORS_H
namespace Adcirc {

enum _genericErrorCodes {
  NoError = 0x0000000,
  MissingFile = 0x0000001,
  FileOpenError = 0x0000002,
  FileReadError = 0x0000003,
  Proj4Error = 0x0000004,
  KdtreeError = 0x0000005
};

namespace Geometry {
enum _GeometryErrorCodes {
  InvalidFormat = 0x0100001,
  UnknownError = 0x0100002
};
}

namespace ModelParameters {
enum _modelParamErrorCodes { MeshMismatch = 0x0400001 };
}
}
#endif  // ADCIRCERRORS_H
