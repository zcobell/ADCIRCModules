# ##############################################################################
# GDAL
# ##############################################################################
option(ENABLE_GDAL "Enabele GDAL" ON)
if(ENABLE_GDAL)
  find_package(GDAL)
  get_filename_component(GDAL_LIBPATH ${GDAL_LIBRARY} DIRECTORY)
  if(NOT GDAL_FOUND)
    message(WARNING "GDAL Library not found. These functions will be disabled.")
  endif(NOT GDAL_FOUND)
else(ENABLE_GDAL)
  message(
    WARNING
      "GDAL Library has been disabled. Raster operations will not be available."
  )
  set(GDAL_FOUND FALSE)
  set(ENABLE_GDAL OFF)
endif(ENABLE_GDAL)
# ##############################################################################
