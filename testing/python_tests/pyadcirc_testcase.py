#!/bin/python
import PyAdcirc
import math

print "Reading a mesh"
m = PyAdcirc.Mesh("../testing/test_files/ms-riv.grd")
m.read()

print "Mesh read successfully"

print "  Node at position 2"
print "  Node 2 id:",m.node(1).id()
print "  Node 2 location: ",m.node(1).x(),",",m.node(1).y(),",",m.node(1).z()
print "  Shifting node..."
m.node(1).setX(-77.045)
print "  New node location: ",m.node(1).x(),",",m.node(1).y(),",",m.node(1).z()
print "  Generating a search tree for the mesh..."
m.buildNodalSearchTree()
print "  Nodal searh tree generated."
print "  Searching for a node..."
index = m.nodalSearchTree().findNearest(-95.0,27.0)
print "  Found index: ",index
print "  Projecting mesh to utm-15 (epsg 26915)..."
m.reproject(26916)
print "  Mesh projected successfully."
print "  New node position: ",m.node(1).x(),",",m.node(1).y(),",",m.node(1).z()
print "  Writing the utm-15 mesh"
m.write("ms-riv-utm15.grd")
print "  Mesh written in utm-15 successfully"
print "  Writing mesh as a shapefile..."
m.toNodeShapefile("ms-riv-utm15.shp");
print "  Mesh written in utm-15 shapefile successfully"
print "  Getting mesh xyz as python array..."
xyz=m.xyz();
print "  Accessing data in python array, node position is: ",xyz[0][1],",",xyz[1][1],",",xyz[2][1]
print "Reading a fort.13"
f = PyAdcirc.NodalAttributes("../testing/test_files/ms-riv.13",m)
f.read()
print "Fort13 read successfully"
print "  Manning n at node",m.node(1).id(),": ",f.attribute("mannings_n_at_sea_floor",1).value(0)
print "  Tau0 at node",m.node(1).id(),": ",f.attribute("primitive_weighting_in_continuity_equation",1).value(0)
print "Reading an adcirc ascii output file"
o = PyAdcirc.OutputFile("../testing/test_files/maxele.63")
o.open();
o.read();
o.close()
print "   Output value for record 1 at node 42: ",o.data(0).z(41)
print "Adcirc maxele ascii file read successfully"
print "Reading netcdf maxele file"
onc = PyAdcirc.OutputFile("../testing/test_files/maxele.63.nc")
onc.open()
onc.read()
onc.close();
print "   Output value for reacord 1 at node 42: ",onc.data(0).z(41)
print "Reading ascii timeseries scalar file"
asc = PyAdcirc.OutputFile("../testing/test_files/sparse_fort.63")
asc.open();
asc.read();
asc.read();
asc.read();
print "    Output value for record 3 at node 925: ",asc.data(2).z(1220);
asc.close();
print "Reading timeseries netcdf vector file"
ncv=PyAdcirc.OutputFile("../testing/test_files/fort.64.nc")
ncv.open();
ncv.read();
ncv.read();
ncv.clearAt(1); #...deletes record 2 from memory
ncv.read();
print "    Velocity at node 1221: ",ncv.data(2).u(1220),",",ncv.data(2).v(1220)
print "       Magnitude is: ",ncv.data(2).magnitude(1220)
print "       Direction is: ",ncv.data(2).direction(1220,PyAdcirc.Radians), "radians or ",ncv.data(2).direction(1220,PyAdcirc.Degrees)," degrees"
print "Success reading netcdf vector timeseries file"
ncv.close()
print "Reading harmonics elevation file"
he = PyAdcirc.HarmonicsOutput("../testing/test_files/fort.53");
he.read();
print "Harmonics elevation read successfully"
print "   M2 amplitude at node 1: ",he.amplitude("M2").value(he.nodeIdToArrayIndex(1))
print "   M2 phase at node 1: ",he.phase("M2").value(he.nodeIdToArrayIndex(1))
print "   K1 amplitude at node 1: ",he.amplitude("K1").value(he.nodeIdToArrayIndex(1))
print "   K1 phase at node 1: ",he.phase("K1").value(he.nodeIdToArrayIndex(1))
print "Reading harmonics velocity file"
hv = PyAdcirc.HarmonicsOutput("../testing/test_files/fort.54");
hv.read();
print "Harmonics velocity read successfully"
print "   M2 u magnitude at node 1: ",hv.u_magnitude("M2").value(hv.nodeIdToArrayIndex(1))
print "   M2 u phase at node 1: ",hv.u_phase("M2").value(hv.nodeIdToArrayIndex(1))
print "   M2 v magnitude at node 1: ",hv.v_magnitude("M2").value(hv.nodeIdToArrayIndex(1))
print "   M2 v phase at node 1: ",hv.v_phase("M2").value(hv.nodeIdToArrayIndex(1))
