# ADCIRCModules
Implementation of routines used to manipulate ADCIRC model files in C++

## Note
This project is still under development.

[![Build Status](https://travis-ci.org/zcobell/ADCModules.svg?branch=master)](https://travis-ci.org/zcobell/ADCModules)
[![Coverity](https://scan.coverity.com/projects/14684/badge.svg)](https://scan.coverity.com/projects/zcobell-adcmodules)

### Credits
| Library  | Function | Source |
|----------|----------|--------|
| KDTREE2  | Used for efficient nearest neighbor searches | Matthew B. Kennel, Institute for Nonlinear Science, UCSD (2004) |
| Boost    | Used as part of KDTREE2 | [Visit Website](http://www.boost.org/) |
| Proj4    | Used to project to various coordinate systems |[Visit Website](https://trac.osgeo.org/proj/) |
| netCDF   | Used to read ADCIRC files formatted with this library | [Visit Website](http://www.unidata.ucar.edu/software/netcdf/) |
| shapelib | Used to convert meshes to shapefile format | [Visit Website](http://shapelib.maptools.org/) |
| swig     | Used to generate an interface to Python | [Visit Website](http://www.swig.org/) |
