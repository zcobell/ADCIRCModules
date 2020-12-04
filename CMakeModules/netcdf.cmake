# ##############################################################################
# NETCDF
# ##############################################################################
if(WIN32)
  set(NETCDF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/netcdf/include")
else(WIN32)
  if(NOT "${NETCDFHOME}" STREQUAL "")
    set(NETCDF_DIR
        "${NETCDFHOME}"
        CACHE STRING "Location of NETCDF Library")
  elseif(NOT $ENV{NETCDFHOME} STREQUAL "")
    set(NETCDF_DIR
        $ENV{NETCDFHOME}
        CACHE STRING "Locatin of NETCDF Library")
  endif(NOT "${NETCDFHOME}" STREQUAL "")

  find_package(NetCDF)

  set(NETCDF_AdditionalLibs
      ""
      CACHE STRING "Additional libraries that may be required for netCDF")

  if(NOT NETCDF_FOUND)
    message(SEND_ERROR "Specify the netCDF path on the following screen")
  else(NOT NETCDF_FOUND)

    set(netcdf_c_code
        "
		#include <netcdf.h>
		int main(){
			int ncid,varid;
			int ierr = nc_def_var_deflate(ncid,varid,1,2,2);
			return 0;
		}
	")
    file(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/netcdf_c_check.c"
         "${netcdf_c_code}")
    try_compile(
      NC_DEFLATE_FOUND "${CMAKE_CURRENT_BINARY_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/netcdf_c_check.c"
      CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${NETCDF_INCLUDE_DIRS}"
      LINK_LIBRARIES "${NETCDF_LIBRARIES}"
      LINK_LIBRARIES "${NETCDF_AdditionalLibs}"
      OUTPUT_VARIABLE LOG1)

    if(NC_DEFLATE_FOUND)
      set(NETCDF_LINKER_FLAG "${NETCDF_LIBRARIES}")
    else(NC_DEFLATE_FOUND)
      message(SEND_ERROR "The netCDF library is not functional.")
    endif(NC_DEFLATE_FOUND)
  endif(NOT NETCDF_FOUND)
endif(WIN32)
# ##############################################################################
