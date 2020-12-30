# ADCIRCModules
Implementation of routines used to manipulate ADCIRC model files in C++. The code provides a C++ interface as well as a Python interface.

[![Build Status](https://travis-ci.com/zcobell/ADCIRCModules.svg?branch=master)](https://travis-ci.com/zcobell/ADCIRCModules)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/4a92371846ec42a48d0aab66ec4a0a3a)](https://www.codacy.com/app/zachary.cobell/ADCModules?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=zcobell/ADCModules&amp;utm_campaign=Badge_Grade)
[![codecov](https://codecov.io/gh/zcobell/ADCIRCModules/branch/master/graph/badge.svg)](https://codecov.io/gh/zcobell/ADCIRCModules)
[![License: GPL v3](https://img.shields.io/badge/License-GPL%20v3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

## Python
When the python interface is compiled, you will have an ADCIRCModules python library file, or pyadcircmodules. For windows, this will by `pyadcircmodules.dll`, `_pyadcircmodules.dylib` on Mac systems, and Linux type systems this will be `_pyadcircmodules.so`. In addition to the library, you will see `pyadcircmodules.py`. CMake will attempt to install these files to the appropriate directory, however, the user can change this. 

```
import pyadcircmodules
m = pyadcircmodules.Mesh("mymesh.grd")
m.read()
```
## Documentation
There is a Doxygen site available [here](http://zcobell.github.io/ADCIRCModules/). The documentation is not yet complete, however, it is slowly progressing. The Python interface is not explicitly documentated, however, the Python interaction is generated using SWIG, so the function calls are identical. 

## Compiling
The build system for this project is CMake

### Submodules
Submodules are used in this project. You should initialize the repository using:
```
git submodule update --init
```

## Note
This project is still under development.

### Citation
Please appropriately cite this work in publications, reports, and other source code.

Suggested citation:
```
@misc{cobell2019adcircmodules,
  title        = {{ADCIRCM}odules: a {C}++ and {P}ython interface for manipulation of {ADCIRC} model data},
  author       = {Cobell, Zachary},
  howpublished = {\url{https://github.com/zcobell/ADCIRCModules}},
  year         = {2020}
}
```

### Credits
| Library  | Function | Source | Included |
|----------|----------|--------|----------|
| Abseil   | Abseil is an open-source collection of C++ code (compliant to C++11) designed to augment the C++ standard library. | [Visit Website](https://abseil.io/) | yes, as submodule |
| Boost    | Used for fast file I/O | [Visit Website](http://www.boost.org/) | yes |
| cxxopts  | Lightweight command line parsing for c++ | [Visit Website](https://github.com/jarro2783/cxxopts) | yes | 
| Date     | Howard Hinnant's date library for c++ | [Visit Website](https://github.com/HowardHinnant/date) | yes, as submodule |
| gdal     | Used to read raster data to interpolate to meshes | [Visit Website](https://www.gdal.org/) | no |
| HDF5     | High-performance data management and storage suite | [Visit Website](https://www.hdfgroup.org/solutions/hdf5/) | no |
| nanoflann | Used for kd-tree searches | [Visit Website](https://github.com/jlblancoc/nanoflann) | yes, as submodule | 
| netCDF   | Used to read ADCIRC files formatted with this library | [Visit Website](http://www.unidata.ucar.edu/software/netcdf/) | no |
| proj     | PROJ is a generic coordinate transformation software that transforms geospatial coordinates from one coordinate reference system (CRS) to another | [Visit Website](https://proj.org/) | yes, but can use external |
| shapelib | Used to convert meshes to shapefile format | [Visit Website](http://shapelib.maptools.org/) | yes |
| swig     | Used to generate an interface to Python | [Visit Website](http://www.swig.org/) | no |
| indicators | Use for pretty progress bars throughout the code | [Visit Website](https://github.com/p-ranav/indicators) | yes |
