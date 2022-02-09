# ##############################################################################
# HDF5
# ##############################################################################
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif(POLICY CMP0074)
if(NOT "${HDF5HOME}" STREQUAL "")
  set(HDF5_ROOT
      "${HDF5HOME}"
      CACHE STRING "Location of HDF5 Library")
elseif(NOT $ENV{HDF5HOME} STREQUAL "")
  set(HDF5_ROOT
      $ENV{HDF5HOME}
      CACHE STRING "Location of HDF5 Library")
endif(NOT "${HDF5HOME}" STREQUAL "")
find_package(HDF5 COMPONENTS C HL REQUIRED)
mark_as_advanced(HDF5_C_LIBRARY_dl      ) 
mark_as_advanced(HDF5_C_LIBRARY_hdf5    ) 
mark_as_advanced(HDF5_C_LIBRARY_m       ) 
mark_as_advanced(HDF5_C_LIBRARY_pthread ) 
mark_as_advanced(HDF5_C_LIBRARY_sz      ) 
mark_as_advanced(HDF5_C_LIBRARY_z       ) 

# ##############################################################################
