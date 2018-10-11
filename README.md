# ADCIRCModules
Implementation of routines used to manipulate ADCIRC model files in C++. The code provides a C++ interface as well as a Python interface.

[![Build Status](https://travis-ci.org/zcobell/ADCIRCModules.svg?branch=master)](https://travis-ci.org/zcobell/ADCIRCModules)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/4a92371846ec42a48d0aab66ec4a0a3a)](https://www.codacy.com/app/zachary.cobell/ADCModules?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=zcobell/ADCModules&amp;utm_campaign=Badge_Grade)
[![codecov](https://codecov.io/gh/zcobell/ADCIRCModules/branch/master/graph/badge.svg)](https://codecov.io/gh/zcobell/ADCIRCModules)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Python
When the python interface is compiled, you will have a PyAdcirc library file. For windows, this will by `PyAdcirc.dll` and for Linux type systems this will be `_PyAdcirc.so`. In addition to the library, you will see `PyAdcirc.py`. CMake will attempt to install these files to the appropriate directory, however, the user can change this. The standard library (`adcircmodules.dll` or `libadcircmodules.so` is also required to be in a location where it can be found at runtime (`LD_LIBRARY_PATH` on Unix systems).

```
import PyAdcirc
m = PyAdcirc.Mesh("mymesh.grd")
m.read()
```
## Documentation
There is a Doxygen site available [Here](http://zcobell.github.io/ADCIRCModules/). The documentation is not yet complete, however, it is slowly progressing. The Python interface is not explicitly documentated, however, the Python interaction is generated using SWIG, so the function calls are identical. 

## Note
This project is still under development.

### Citation
Please appropriately cite this work in publications, reports, and other source code.

Suggested citation:
```
Cobell, Z. (2018). Adcirc Modules (Version xxx) [Library]. Retrieved from https://github.com/zcobell/ADCIRCModules
```


### Credits
| Library  | Function | Source |
|----------|----------|--------|
| KDTREE2  | Used for efficient nearest neighbor searches | Matthew B. Kennel, Institute for Nonlinear Science, UCSD (2004) |
| Boost    | Used by KDTREE2 and for fast file I/O | [Visit Website](http://www.boost.org/) |
| Proj4    | Used to project to various coordinate systems |[Visit Website](https://trac.osgeo.org/proj/) |
| netCDF   | Used to read ADCIRC files formatted with this library | [Visit Website](http://www.unidata.ucar.edu/software/netcdf/) |
| shapelib | Used to convert meshes to shapefile format | [Visit Website](http://shapelib.maptools.org/) |
| swig     | Used to generate an interface to Python | [Visit Website](http://www.swig.org/) |
| gdal     | Used to read raster data to interpolate to meshes | [GDAL](https://www.gdal.org/) |
