# ADCIRCModules
Implementation of routines used to manipulate ADCIRC model files in C++. The code provides a C++ interface as well as a Python interface.

## Python
When the python interface is compiled, you will have a PyAdcirc library file. For windows, this will by `PyAdcirc.dll` and for Linux type systems this will be `_PyAdcirc.so`. In addition to the library, you will see `PyAdcirc.py`. CMake will attempt to install these files to the appropriate directory, however, the user can change this. The standard library (`adcircmodules.dll` or `adcircmodules.so` is also required to be in a location where it can be found at runtime (`LD_LIBRARY_PATH` on Unix systems).

```
import PyAdcirc
m = PyAdcirc.Mesh("mymesh.grd")
error = m.read()
```

## Note
This project is still under development.

[![Build Status](https://travis-ci.org/zcobell/ADCModules.svg?branch=master)](https://travis-ci.org/zcobell/ADCModules)
[![Coverity](https://scan.coverity.com/projects/14684/badge.svg)](https://scan.coverity.com/projects/zcobell-adcmodules)

### Credits
| Library  | Function | Source |
|----------|----------|--------|
| KDTREE2  | Used for efficient nearest neighbor searches | Matthew B. Kennel, Institute for Nonlinear Science, UCSD (2004) |
| Boost    | Used by KDTREE2 and for fast file I/O | [Visit Website](http://www.boost.org/) |
| Proj4    | Used to project to various coordinate systems |[Visit Website](https://trac.osgeo.org/proj/) |
| netCDF   | Used to read ADCIRC files formatted with this library | [Visit Website](http://www.unidata.ucar.edu/software/netcdf/) |
| shapelib | Used to convert meshes to shapefile format | [Visit Website](http://shapelib.maptools.org/) |
| swig     | Used to generate an interface to Python | [Visit Website](http://www.swig.org/) |
