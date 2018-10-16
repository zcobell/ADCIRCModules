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
