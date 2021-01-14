#!/usr/bin/env python

import pyadcircmodules as pa

m = pa.Mesh("ms-riv.grd")
n = pa.NodalAttributes("ms-riv.13")

m.read()
n.read()

m.defineProjection(4326,True)

idx = n.locateAttribute("surface_directional_effective_roughness_length")

r = pa.Griddata(m,"ccap_conus_2016_landcover2.tif",26915)
r.setShowProgressBar(True)
r.readLookupTable("CCAP_DWind.table")
r.setInterpolationFlags(pa.Average)

dw = r.computeDirectionalWindReduction(True)

for i in range(m.numNodes()):
    n.attribute(idx,i).setValue(dw[i])

n.write("new.13")

