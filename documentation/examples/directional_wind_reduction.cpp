#include <iostream>
#include <memory>
#include "adcircmodules.h"

int main() {
  using namespace Adcirc::Geometry;
  using namespace Adcirc::Interpolation;
  using namespace Adcirc::ModelParameters;

  // Generate the mesh object and read the file
  auto m = std::make_unique<Mesh>("../binera25m_wFP_tide_only.14");
  m->read();
  m->defineProjection(4326, true);

  // Generate the nodal attributes object, read the file, and find our target
  // attribute
  auto fort13 = std::make_unique<NodalAttributes>(
      "../binera25m_wFP_tide_only.13", m.get());
  fort13->read();
  auto midx2 =
      fort13->locateAttribute("surface_directional_effective_roughness_length");

  // Generate the griddata object with the raster and specify the epsg of the
  // coordinate system
  auto g = std::make_unique<Griddata>(m.get(), "consu_epsg26918.tif", 26918);

  // For testing, you may only want to do a subset of the model nodes. This loop
  // will only calculate the first 50 points. Omit this loop to do the entire
  // mesh
  for (auto i = 0; i < m->numNodes(); ++i) {
    if (i < 50) {
      g->setInterpolationFlag(i, Adcirc::Interpolation::Average);
    } else {
      g->setInterpolationFlag(i, Adcirc::Interpolation::NoMethod);
    }
  }

  // Show the progress bar during the calculation
  g->setShowProgressBar(true);

  // Specify the lookup table
  g->readLookupTable("../CCAP_DWind.table");

  // For large rasters, do not put the entire raster into memory. Instead
  // read from the disk ask required
  g->setRasterInMemory(false);

  // Compute the directional wind reduction using a lookup table. The use
  // of a lookup table is specified using the [true] parameter
  auto r = g->computeDirectionalWindReduction(true);

  // The return of the above command is just a 2d vector. Now, put the
  // values into the fort.13 object
  for (auto i = 0; i < m->numNodes(); ++i) {
    fort13->attribute(midx2, i)->setValue(r[i]);
  }

  // Write the fort.13 to disk
  fort13->write("../binera25m_wFP_w_dir_ws.13");

  return 0;
}
