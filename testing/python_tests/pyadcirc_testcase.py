#!/bin/python
import PyAdcirc

print "Reading a mesh"
m = PyAdcirc.Mesh("../testing/test_files/ms-riv.grd")
ierr = m.read()
if ierr != 0:
    exit(ierr)

print "Mesh read successfully"

print "  Node at position 2"
print "  Node 2 id:",m.node(1).id()
print "  Node 2 location: ",m.node(1).x(),",",m.node(1).y(),",",m.node(1).z()
print "  Shifting node..."
m.node(1).setX(-77.045)
print "  New node location: ",m.node(1).x(),",",m.node(1).y(),",",m.node(1).z()
print "  Generating a search tree for the mesh..."
ierr = m.buildNodalSearchTree()
if ierr != 0:
    exit(ierr)
print "  Nodal searh tree generated."
print "  Searching for a node..."
index = m.nodalSearchTree().findNearest(-95.0,27.0)
print "  Found index: ",index
print "  Projecting mesh to utm-15 (epsg 26915)..."
ierr = m.reproject(26916)
if ierr != 0:
    exit(ierr)
print "  Mesh projected successfully."
print "  New node position: ",m.node(1).x(),",",m.node(1).y(),",",m.node(1).z()
print "  Writing the utm-15 mesh"
ierr = m.write("ms-riv-utm15.grd")
if ierr != 0:
    exit(ierr)
print "  Mesh written in utm-15 successfully"
print "  Writing mesh as a shapefile..."
ierr = m.toShapefile("ms-riv-utm15.shp");
if ierr != 0:
    exit(ierr)
print "  Mesh written in utm-15 shapefile successfully"

print "Reading a fort.13"
f = PyAdcirc.NodalAttributes("../testing/test_files/ms-riv.13",m)
ierr = f.read()
if ierr != 0:
    exit(ierr)
print "Fort13 read successfully"
print "  Manning n at node",m.node(1).id(),": ",f.attribute("mannings_n_at_sea_floor",1).value(0)
print "  Tau0 at node",m.node(1).id(),": ",f.attribute("primitive_weighting_in_continuity_equation",1).value(0)
