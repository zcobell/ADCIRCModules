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
#ifndef MESHFORMATS_H
#define MESHFORMATS_H

namespace Adcirc {

namespace Geometry {

enum MeshFormat {
  /// Unknown mesh format
  MESH_UNKNOWN,
  /// ADCIRC mesh format (*.14, *.grd)
  MESH_ADCIRC,
  /// Aquaveo generic mesh format (*.2dm)
  MESH_2DM,
  /// Deltares D-Flow FM format (*_net.nc)
  MESH_DFLOW
};

}

}  // namespace Adcirc

#endif  // MESHFORMATS_H
