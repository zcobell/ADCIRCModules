# ##############################################################################
# CMake Build File for ADCIRCMODULES
#
# Written By: Zach Cobell
#
# ##############################################################################
#
# The CMake build system enable ADCIRCModules to be deployed and built in a
# cross platform environment.
#
# ##############################################################################

# ...Set the default build type
if(DEFINED CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      ${CMAKE_BUILD_TYPE}
      CACHE STRING "Choose the type of
        build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
        Release RelWithDebInfo MinSizeRel.")
elseif(COVERAGE)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE
        STRING
        "Choose the type of build,
            options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
            RelWithDebInfo MinSizeRel.")
else()
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING "Choose the type of build,
        options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
        RelWithDebInfo MinSizeRel.")
endif()

# ##############################################################################
# GENERAL OPTIONS
# ##############################################################################
cmake_minimum_required(VERSION 2.8.12)
project(ADCIRCModules)
enable_language(C)
enable_language(CXX)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
# ##############################################################################

# ##############################################################################
# Enable running tests
# ##############################################################################
if(UNIX OR CYGWIN)
  enable_testing()
endif(UNIX OR CYGWIN)
# ##############################################################################

# ##############################################################################
# Enable Coverage
# ##############################################################################
# OPTION(COVERAGE "Export Code Coverage report from tests" OFF)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
if(COVERAGE)
  if(CMAKE_COMPILER_IS_GNUCXX)
    include(CodeCoverage)
    setup_target_for_coverage(adcircmodules_coverage tests coverage)
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -g -O0 -fprofile-arcs -ftest-coverage"
    )
  endif(CMAKE_COMPILER_IS_GNUCXX)
endif(COVERAGE)
# ##############################################################################

# ##############################################################################
# C++ 11/14 Check
# ##############################################################################
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
else(COMPILER_SUPPORTS_CXX14)
  check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
  else(COMPILER_SUPPORTS_CXX11)
    message(
      FATAL_ERROR
        "The compiler ${CMAKE_CXX_COMPILER} has no C++11/14 support. Please use a different C++ compiler."
    )
  endif(COMPILER_SUPPORTS_CXX11)
endif(COMPILER_SUPPORTS_CXX14)
# ##############################################################################

# ##############################################################################
# Compiler flags
# ##############################################################################
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_RELEASE)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_DEBUG)
mark_as_advanced(CLEAR CMAKE_C_FLAGS_RELEASE)
mark_as_advanced(CLEAR CMAKE_C_FLAGS_DEBUG)
mark_as_advanced(CLEAR CMAKE_CXX_COMPILER)
mark_as_advanced(CLEAR CMAKE_C_COMPILER)
# ##############################################################################

# ##############################################################################
# LIBRARY VERSION
# ##############################################################################
set(ADCIRCMODULES_VERSION_MAJOR 0)
set(ADCIRCMODULES_VERSION_MINOR 3)
set(ADCIRCMODULES_VERSION_PATCH 0)
set(ADCIRCMODULES_VERSION_STRING
    ${ADCIRCMODULES_VERSION_MAJOR}.${ADCIRCMODULES_VERSION_MINOR}.${ADCIRCMODULES_VERSION_PATCH}
)
# ##############################################################################

# ##############################################################################
# CODE VERSION (GIT)
# ##############################################################################
execute_process(
  COMMAND git describe --always --tags
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
  RESULT_VARIABLE GIT_RETURN_VALUE
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT "${GIT_RETURN_VALUE}" STREQUAL "0")
  set(GIT_VERSION "${ADCIRCMODULES_VERSION_STRING}.cv")
endif()
message(STATUS "ADCIRCModules Version: ${GIT_VERSION}")
# ##############################################################################

# ##############################################################################
# ABSEIL SWISS TABLES
# ##############################################################################
if(NOT CYGWIN)
  option(USE_GOOGLE_FLAT_MAP "Use Google's hash table to increase speed" ON)
  if(USE_GOOGLE_FLAT_MAP)
    if(MSVC)
      # /wd4005  macro-redefinition /wd4068  unknown pragma /wd4244  conversion
      # from 'type1' to 'type2' /wd4267  conversion from 'size_t' to 'type2'
      # /wd4800  force value to bool 'true' or 'false' (performance warning)
      add_compile_options(/wd4005 /wd4068 /wd4244 /wd4267 /wd4800)
      add_definitions(/DNOMINMAX /DWIN32_LEAN_AND_MEAN=1
                      /D_CRT_SECURE_NO_WARNINGS)
    endif(MSVC)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/abseil EXCLUDE_FROM_ALL)
    message(
      STATUS
        "Using Google's Flat Map (from ABSL) instead of unordered_map for performance"
    )
  endif(USE_GOOGLE_FLAT_MAP)
else(NOT CYGWIN)
  set(USE_GOOGLE_FLAT_MAP FALSE)
endif(NOT CYGWIN)
# ##############################################################################

# ##############################################################################
# Boost
# ##############################################################################
find_package(Boost 1.60.0)
if(NOT Boost_FOUND OR PACKAGE_VERSION_UNSUITABLE)
  message(STATUS "Using included Boost Version: 1.66.0")
  set(Boost_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_66_0)
endif()
# ##############################################################################

# ##############################################################################
# TESTING
# ##############################################################################
option(BUILD_TESTS "Build test cases" OFF)
# ##############################################################################

# ##############################################################################
# SET THE LOCATION OF TEMPORARY STATIC LIBS
# ##############################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles)
# ##############################################################################

# ##############################################################################
# SWIG/PYTHON/PERL Note: This will attempt to default to Python3 Give the flag
# -DPYTHON2=ON to force Python2 usage
# ##############################################################################
if(${PYTHON2})
  message(STATUS "CMake will attempt to use the Python2 interface only")
  find_package(PythonInterp 2.7 REQUIRED)
  find_package(PythonLibs 2.7 REQUIRED)
  set(PYTHONTYPE 2)
else()
  find_package(PythonInterp 3.0)
  find_package(PythonLibs 3.0)
  if(NOT PythonLibs_FOUND AND NOT PythonInterp_FOUND)
    find_package(PythonInterp 2.7)
    find_package(PythonLibs 2.7)
    set(PYTHONTYPE 2)
  else()
    set(PYTHONTYPE 3)
  endif()
endif()

find_package(SWIG 3.0)
if(SWIG_FOUND
   AND PythonLibs_FOUND
   AND PythonInterp_FOUND)
  include(${SWIG_USE_FILE})
  if(PythonLibs_FOUND)
    option(PYTHON_TESTING "Test the Python Libraries with CTest" OFF)
    execute_process(
      COMMAND ${PYTHON_EXECUTABLE} -m site --user-site
      OUTPUT_VARIABLE temp_PYTHON_SITE_DIRECTORY
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(PYTHON_INSTALL_DIRECTORY
        ${temp_PYTHON_SITE_DIRECTORY}
        CACHE STRING "Location where Python libraries will be installed")
    mark_as_advanced(CLEAR PYTHON_EXECUTABLE)
  else(PythonLibs_FOUND)
    set(PYTHON_INSTALL_DIRECTORY
        "PYTHON_INSTALL_DIRECTORY-NOTFOUND"
        CACHE STRING "Location where Python libraries will be installed")
  endif(PythonLibs_FOUND)

  # FIND_PACKAGE(PerlLibs) IF(PerlLibs_FOUND) OPTION(TEST_PERL "Test the perl
  # build for SWIG. Note: Does not work yet" OFF) ENDIF(PerlLibs_FOUND)
else(
  SWIG_FOUND
  AND PythonLibs_FOUND
  AND PythonInterp_FOUND)
  message(
    WARNING "SWIG/Python libraries could not be found. PyAdcirc disabled.")
endif(
  SWIG_FOUND
  AND PythonLibs_FOUND
  AND PythonInterp_FOUND)
# ##############################################################################

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
find_package(HDF5)
# ##############################################################################

# ##############################################################################
# OpenMP
# ##############################################################################
option(ENABLE_OPENMP "Enable OpenMP Multithreading" ON)
if(ENABLE_OPENMP)
  find_package(OpenMP)
else(ENABLE_OPENMP)

endif(ENABLE_OPENMP)
# ##############################################################################

# ##############################################################################
# OpenSSL
# ##############################################################################
if(NOT "${OPENSSLHOME}" STREQUAL "")
  set(OPENSSL_ROOT_DIR "${OPENSSLHOME}")
elseif(NOT $ENV{OPENSSLHOME} STREQUAL "")
  set(OPENSSL_ROOT_DIR $ENV{OPENSSLHOME})
endif(NOT "${OPENSSLHOME}" STREQUAL "")
find_package(OpenSSL)
if(OpenSSL_FOUND)
  message(STATUS "OpenSSL libraries found. Hashing is enabled.")
else()
  message(STATUS "OpenSSL libraries not found. Hashing disabled.")
endif()
# ##############################################################################

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
  OPTION(GDAL_IS_THREADSAFE "Assume that GDAL is threadsafe" OFF)
else(ENABLE_GDAL)
  message(
    WARNING
      "GDAL Library has been disabled. Raster operations will not be available."
  )
  set(GDAL_FOUND FALSE)
  set(ENABLE_GDAL OFF)
endif(ENABLE_GDAL)
# ##############################################################################

# ##############################################################################
# ##############################################################################
# LIBRARIES
# ##############################################################################
# ##############################################################################

# ##############################################################################
# EzProj Library
# ##############################################################################
add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/ezproj EXCLUDE_FROM_ALL)
# ##############################################################################

# ##############################################################################
# Shapelib Library
# ##############################################################################
add_library(
  shapelib STATIC
  ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/shpopen.c
  ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/dbfopen.c
  ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/safileio.c
  ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/shptree.c)
target_include_directories(shapelib
                           PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/shapelib)
# ##############################################################################

# ##############################################################################
# ADCIRCModules Library
# ##############################################################################
set(ADCIRCMODULES_SOURCES
    ${CMAKE_SOURCE_DIR}/src/config.cpp
    ${CMAKE_SOURCE_DIR}/src/logging.cpp
    ${CMAKE_SOURCE_DIR}/src/filetypes.cpp
    ${CMAKE_SOURCE_DIR}/src/formatting.cpp
    ${CMAKE_SOURCE_DIR}/src/mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/node.cpp
    ${CMAKE_SOURCE_DIR}/src/element.cpp
    ${CMAKE_SOURCE_DIR}/src/hash.cpp
    ${CMAKE_SOURCE_DIR}/src/hmdf.cpp
    ${CMAKE_SOURCE_DIR}/src/hmdfstation.cpp
    ${CMAKE_SOURCE_DIR}/src/stationinterpolationoptions.cpp
    ${CMAKE_SOURCE_DIR}/src/stationinterpolation.cpp
    ${CMAKE_SOURCE_DIR}/src/netcdftimeseries.cpp
    ${CMAKE_SOURCE_DIR}/src/hash_private.cpp
    ${CMAKE_SOURCE_DIR}/src/cdate.cpp
    ${CMAKE_SOURCE_DIR}/src/boundary.cpp
    ${CMAKE_SOURCE_DIR}/src/fileio.cpp
    ${CMAKE_SOURCE_DIR}/src/stringconversion.cpp
    ${CMAKE_SOURCE_DIR}/src/nodalattributes.cpp
    ${CMAKE_SOURCE_DIR}/src/attribute.cpp
    ${CMAKE_SOURCE_DIR}/src/attributemetadata.cpp
    ${CMAKE_SOURCE_DIR}/src/outputmetadata.cpp
    ${CMAKE_SOURCE_DIR}/src/outputrecord.cpp
    ${CMAKE_SOURCE_DIR}/src/readoutput.cpp
    ${CMAKE_SOURCE_DIR}/src/writeoutput.cpp
    ${CMAKE_SOURCE_DIR}/src/harmonicsrecord.cpp
    ${CMAKE_SOURCE_DIR}/src/harmonicsoutput.cpp
    ${CMAKE_SOURCE_DIR}/src/elementtable.cpp
    ${CMAKE_SOURCE_DIR}/src/nodetable.cpp
    ${CMAKE_SOURCE_DIR}/src/meshchecker.cpp
    ${CMAKE_SOURCE_DIR}/src/multithreading.cpp
    ${CMAKE_SOURCE_DIR}/src/constants.cpp
    ${CMAKE_SOURCE_DIR}/src/mesh_private.cpp
    ${CMAKE_SOURCE_DIR}/src/kdtree.cpp
    ${CMAKE_SOURCE_DIR}/src/kdtree_private.cpp
    ${CMAKE_SOURCE_DIR}/src/topology.cpp
    ${CMAKE_SOURCE_DIR}/src/facetable.cpp
    ${CMAKE_SOURCE_DIR}/src/harmonicsoutput_private.cpp
    ${CMAKE_SOURCE_DIR}/src/harmonicsrecord_private.cpp
    ${CMAKE_SOURCE_DIR}/src/nodalattributes_private.cpp)

if(GDAL_FOUND)
  set(ADCIRCMODULES_SOURCES
      ${ADCIRCMODULES_SOURCES} ${CMAKE_SOURCE_DIR}/src/griddata.cpp
      ${CMAKE_SOURCE_DIR}/src/griddata_private.cpp
      ${CMAKE_SOURCE_DIR}/src/pixel.cpp ${CMAKE_SOURCE_DIR}/src/rasterdata.cpp)
endif(GDAL_FOUND)

add_library(adcircmodules SHARED ${ADCIRCMODULES_SOURCES})

set(HEADER_LIST
    ${CMAKE_SOURCE_DIR}/src/adcircmodules.h
    ${CMAKE_SOURCE_DIR}/src/adcircmodules_global.h
    ${CMAKE_SOURCE_DIR}/src/attribute.h
    ${CMAKE_SOURCE_DIR}/src/attributemetadata.h
    ${CMAKE_SOURCE_DIR}/src/boundary.h
    ${CMAKE_SOURCE_DIR}/src/config.h
    ${CMAKE_SOURCE_DIR}/src/element.h
    ${CMAKE_SOURCE_DIR}/src/hash.h
    ${CMAKE_SOURCE_DIR}/src/hashtype.h
    ${CMAKE_SOURCE_DIR}/src/hmdf.h
    ${CMAKE_SOURCE_DIR}/src/hmdfstation.h
    ${CMAKE_SOURCE_DIR}/src/stationinterpolationoptions.h
    ${CMAKE_SOURCE_DIR}/src/stationinterpolation.h
    ${CMAKE_SOURCE_DIR}/src/logging.h
    ${CMAKE_SOURCE_DIR}/src/filetypes.h
    ${CMAKE_SOURCE_DIR}/src/harmonicsoutput.h
    ${CMAKE_SOURCE_DIR}/src/harmonicsrecord.h
    ${CMAKE_SOURCE_DIR}/src/interpolationmethods.h
    ${CMAKE_SOURCE_DIR}/src/fileio.h
    ${CMAKE_SOURCE_DIR}/src/mesh.h
    ${CMAKE_SOURCE_DIR}/src/nodalattributes.h
    ${CMAKE_SOURCE_DIR}/src/node.h
    ${CMAKE_SOURCE_DIR}/src/readoutput.h
    ${CMAKE_SOURCE_DIR}/src/writeoutput.h
    ${CMAKE_SOURCE_DIR}/src/outputrecord.h
    ${CMAKE_SOURCE_DIR}/src/outputmetadata.h
    ${CMAKE_SOURCE_DIR}/src/meshchecker.h
    ${CMAKE_SOURCE_DIR}/src/elementtable.h
    ${CMAKE_SOURCE_DIR}/src/multithreading.h
    ${CMAKE_SOURCE_DIR}/src/constants.h
    ${CMAKE_SOURCE_DIR}/src/kdtree.h
    ${CMAKE_SOURCE_DIR}/src/default_values.h
    ${CMAKE_SOURCE_DIR}/src/cdate.h
    ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src/ezproj.h)

if(GDAL_FOUND)
  target_compile_definitions(adcircmodules PRIVATE "USE_GDAL")
  target_include_directories(adcircmodules PRIVATE ${GDAL_INCLUDE_DIR})
  link_directories(${GDAL_LIBPATH})
  target_link_libraries(adcircmodules ${GDAL_LIBRARY})
  set(HEADER_LIST ${HEADER_LIST} ${CMAKE_SOURCE_DIR}/src/rasterdata.h
                  ${CMAKE_SOURCE_DIR}/src/griddata.h)
  IF(GDAL_IS_THREADSAFE)
    target_compile_definitions( adcircmodules PRIVATE "GDAL_IS_THREADSAFE" )
  ENDIF()
endif(GDAL_FOUND)
target_include_directories(
  adcircmodules
  PRIVATE ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/src/pimpl
          ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src
          ${CMAKE_SOURCE_DIR}/thirdparty/shapelib
          ${CMAKE_SOURCE_DIR}/thirdparty/abseil
          ${CMAKE_SOURCE_DIR}/thirdparty/date/include/date
          ${CMAKE_SOURCE_DIR}/thirdparty/nanoflann/include
          ${Boost_INCLUDE_DIRS}
          ${NETCDF_INCLUDE_DIR})

add_dependencies(adcircmodules ezproj shapelib)

target_compile_definitions(adcircmodules PRIVATE GIT_VERSION="${GIT_VERSION}")
target_compile_definitions(adcircmodules PRIVATE ADCIRCMODULES_LIBRARY)

set_target_properties(
  adcircmodules PROPERTIES VERSION ${ADCIRCMODULES_VERSION_STRING}
                           SOVERSION ${ADCIRCMODULES_VERSION_MAJOR})

set_target_properties(adcircmodules PROPERTIES PUBLIC_HEADER "${HEADER_LIST}")

if(${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local"
   OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local/"
   OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/"
   OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/")
  set(HEADER_DEST "${CMAKE_INSTALL_INCLUDEDIR}/adcircmodules")
else()
  set(HEADER_DEST ${CMAKE_INSTALL_INCLUDEDIR})
endif()

write_basic_package_version_file(
  adcircmodulesConfigVersion.cmake
  VERSION ${ADCIRCMODULES_VERSION_STRING}
  COMPATIBILITY SameMajorVersion)
install(
  TARGETS adcircmodules
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ADCIRCMODULES_RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ADCIRCMODULES_RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT ADCIRCMODULES_DEVELOPMENT
  PUBLIC_HEADER DESTINATION ${HEADER_DEST} COMPONENT ADCIRCMODULES_DEVELOPMENT)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/adcircmodulesConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

set_target_properties(adcircmodules PROPERTIES CMAKE_CXX_VISIBILITY_PRESET
                                               hidden)
set_target_properties(adcircmodules PROPERTIES CMAKE_CXX_INLINES_HIDDEN YES)

target_link_libraries(adcircmodules ezproj shapelib)

if(WIN32)
  link_directories(${CMAKE_SOURCE_DIR}/thirdparty/netcdf/libs_vc64)
  target_link_libraries(adcircmodules netcdf hdf5 hdf5_hl)
else(WIN32)
  target_link_libraries(adcircmodules ${NETCDF_LIBRARIES})
endif(WIN32)

if(OpenSSL_FOUND)
  target_compile_definitions(adcircmodules PRIVATE ADCMOD_HAVE_OPENSSL)
  target_include_directories(adcircmodules PRIVATE ${OPENSSL_INCLUDE_DIR})
  target_link_libraries(adcircmodules ${OPENSSL_CRYPTO_LIBRARY}
                        ${OPENSSL_SSL_LIBRARY})
endif(OpenSSL_FOUND)

if(APPLE)
  set(CMAKE_MACOSX_RPATH 0)
  set_target_properties(adcircmodules PROPERTIES INSTALL_NAME_DIR
                                                 "adcircmodules")
  set_target_properties(adcircmodules PROPERTIES MACOSX_RPATH "adcircmodules")
endif(APPLE)

if(USE_GOOGLE_FLAT_MAP)
  target_link_libraries(adcircmodules absl::flat_hash_map)
  add_dependencies(adcircmodules absl::flat_hash_map)
  target_compile_definitions(adcircmodules PRIVATE USE_GOOGLE_FLAT_MAP)
endif(USE_GOOGLE_FLAT_MAP)

if(OPENMP_FOUND)
  target_compile_options(adcircmodules PRIVATE ${OpenMP_CXX_FLAGS})
  target_link_libraries(adcircmodules ${OpenMP_CXX_LIB_NAMES}
                        ${OpenMP_CXX_FLAGS})
endif(OPENMP_FOUND)

if(HDF5_FOUND)
  target_compile_definitions(adcircmodules PRIVATE HAVE_HDF5
                                                   ${HDF5_DEFINITIONS})
  target_include_directories(adcircmodules PRIVATE ${HDF5_INCLUDE_DIRS})
  target_link_libraries(adcircmodules ${HDF5_LIBRARIES})
endif(HDF5_FOUND)

# ##############################################################################

# ##############################################################################
# Test case
# ##############################################################################
# ##############################################################################

# ##############################################################################
# PyAdcirc Library
# ##############################################################################
if(SWIG_FOUND
   AND PythonLibs_FOUND
   AND PythonInterp_FOUND)

  if(GDAL_FOUND)
    set(SWIG_GDAL_FLAG "-D_USE_GDAL")
  endif(GDAL_FOUND)

  if(PYTHONTYPE EQUAL 2)
    set(PYTHONFLAG "")
  else()
    set(PYTHONFLAG "-py3")
  endif()

  if(WIN32)

    # ...Builds a dummy cmake project Which will add some windows specific
    # include files not required by Unix/Cygwin [mingw64] Further testing
    # required to see if this is required by MSVC compilers
    file(
      WRITE ${CMAKE_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
      "
            FILE(READ \${SRC} DATA)
            FILE(WRITE \${DST} \"#include <math.h>\")
            FILE(APPEND \${DST} \"\\n\\n\")
            FILE(APPEND \${DST} \"\${DATA}\")
        ")

    add_custom_command(
      OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
      COMMAND
        ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python
        ${PYTHONFLAG} -I${CMAKE_SOURCE_DIR}/src
        -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src i -I${PYTHON_INCLUDE_PATH}
        ${SWIG_GDAL_FLAG} -o
        ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1
        ${CMAKE_SOURCE_DIR}/swig/adcirc.i
      COMMAND
        ${CMAKE_COMMAND}
        -DSRC=\"${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1\"
        -DDST=\"${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx\" -P
        ${CMAKE_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating SWIG interface to Python...")

  else(WIN32)
    add_custom_command(
      OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
      COMMAND
        ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python
        ${PYTHONFLAG} -I${CMAKE_SOURCE_DIR}/src
        -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src -I${PYTHON_INCLUDE_PATH}
        ${SWIG_GDAL_FLAG} -o
        ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
        ${CMAKE_SOURCE_DIR}/swig/adcirc.i
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating SWIG interface to Python...")
  endif(WIN32)

  add_library(PyAdcirc SHARED
              ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx)
  target_include_directories(
    PyAdcirc PRIVATE ${PYTHON_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src)
  target_link_libraries(PyAdcirc adcircmodules ${PYTHON_LIBRARY})
  if(OpenSSL_FOUND)
    target_include_directories(PyAdcirc PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(PyAdcirc ${OPENSSL_CRYPTO_LIBRARY}
                          ${OPENSSL_SSL_LIBRARY})
  endif(OpenSSL_FOUND)
  set_target_properties(PyAdcirc PROPERTIES PREFIX "_")
  set_target_properties(PyAdcirc PROPERTIES INSTALL_NAME_DIR "PyAdcirc")
  set_property(
    DIRECTORY
    APPEND
    PROPERTY ADDITIONAL_MAKE_CLEAN_FILES PyAdcirc.py
             CMakeFiles/python_adcirc_wrap.cxx)

  add_dependencies(PyAdcirc adcircmodules)

  if(WIN32)
    set_target_properties(
      PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                          RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    install(
      TARGETS PyAdcirc
      LIBRARY DESTINATION bin
      RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  elseif(CYGWIN)
    set_target_properties(PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                              ${CMAKE_BINARY_DIR})
    install(
      TARGETS PyAdcirc
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  else(WIN32)
    set_target_properties(PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                              ${CMAKE_BINARY_DIR})
    install(TARGETS PyAdcirc LIBRARY DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  endif(WIN32)

  install(FILES ${CMAKE_BINARY_DIR}/PyAdcirc.py
          DESTINATION ${PYTHON_INSTALL_DIRECTORY})

  if(WIN32)
    set_target_properties(PyAdcirc PROPERTIES SUFFIX ".pyd")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      # 64 bits
      target_compile_definitions(PyAdcirc PRIVATE "MS_WIN64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      # 32 bits
      target_compile_definitions(PyAdcirc PRIVATE "MS_WIN32")
    endif()
  elseif(APPLE)
    set_target_properties(PyAdcirc PROPERTIES SUFFIX ".so")
  endif(WIN32)
endif(
  SWIG_FOUND
  AND PythonLibs_FOUND
  AND PythonInterp_FOUND)
# ##############################################################################

# ##############################################################################
# PerlAdcirc Library
# ##############################################################################
# IF(TEST_PERL) IF(Perl_FOUND AND SWIG_FOUND) IF(WIN32)
#
# #...Builds a dummy cmake project #   Which will add some windows specific
# include #   files not required by Unix/Cygwin [mingw64] #   Further testing
# required to see if this is #   required by MSVC compilers FILE(WRITE
# ${CMAKE_BINARY_DIR}/CMakeFiles/perlwrapper.cmake " FILE(READ \${SRC} DATA)
# FILE(WRITE \${DST} \"#include <math.h>\") FILE(APPEND \${DST} \"\\n\\n\")
# FILE(APPEND \${DST} \"\${DATA}\") ")
#
# ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
# COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -perl
# -I${CMAKE_SOURCE_DIR}/src -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src
# -I${PERL_INCLUDE_PATH} -o
# ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx.1
# ${CMAKE_SOURCE_DIR}/swig/adcirc.i COMMAND ${CMAKE_COMMAND}
# -DSRC=${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx.1
# -DDST=${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx -P
# ${CMAKE_BINARY_DIR}/CMakeFiles/perl_wrapper.cmake WORKING_DIRECTORY
# ${CMAKE_BINARY_DIR} COMMENT "Generating SWIG interface to Perl..." )
#
# ELSE(WIN32) ADD_CUSTOM_COMMAND( OUTPUT
# ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx COMMAND ${SWIG_EXECUTABLE}
# -outdir ${CMAKE_BINARY_DIR} -c++ -perl -I${CMAKE_SOURCE_DIR}/src
# -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src -I${PERL_INCLUDE_PATH} -o
# ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
# ${CMAKE_SOURCE_DIR}/swig/adcirc.i WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
# COMMENT "Generating SWIG interface to Perl..." ) ENDIF(WIN32)
#
# ADD_LIBRARY( perladcirc SHARED
# ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx )
# TARGET_INCLUDE_DIRECTORIES( perladcirc PRIVATE ${PERL_INCLUDE_PATH}
# ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src
# ${CMAKE_SOURCE_DIR}/libraries/rasterdata ) TARGET_LINK_LIBRARIES( perladcirc
# PRIVATE adcircmodules ${PERL_LIBRARY} ) SET_TARGET_PROPERTIES( perladcirc
# PROPERTIES PREFIX "") SET_PROPERTY( DIRECTORY APPEND PROPERTY
# ADDITIONAL_MAKE_CLEAN_FILES perladcirc.pm CMakeFiles/perl_adcirc_wrap.cxx )
# ADD_DEPENDENCIES( perladcirc adcircmodules )
#
# TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE HAS_BOOL)
#
# IF(WIN32) IF(CMAKE_SIZEOF_VOID_P EQUAL 8) # 64 bits
# TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE "MS_WIN64")
# ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 4) # 32 bits
# TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE "MS_WIN32") ENDIF() ENDIF(WIN32)
# ENDIF(Perl_FOUND AND SWIG_FOUND) ENDIF(TEST_PERL)
# ##############################################################################

# ##############################################################################
# Test Cases
# ##############################################################################
if(UNIX OR CYGWIN)
  if(BUILD_TESTS)
    # ...C++ Testing
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cxx_testcases)

    set(TEST_LIST
        cxx_readmesh.cpp
        cxx_readnetcdfmesh.cpp
        cxx_writemesh.cpp
        cxx_writeshapefile.cpp
        cxx_projectmesh.cpp
        cxx_nodalSearchTree.cpp
        cxx_elementalSearchTree.cpp
        cxx_readfort13_wmesh.cpp
        cxx_readfort13_womesh.cpp
        cxx_fort13findatt.cpp
        cxx_readasciifull.cpp
        cxx_readasciisparse.cpp
        cxx_readmaxele.cpp
        cxx_readnetcdfmaxele.cpp
        cxx_readasciivector.cpp
        cxx_readnetcdf.cpp
        cxx_readnetcdfvector.cpp
        cxx_readHarmonicsElevation.cpp
        cxx_readHarmonicsVelocity.cpp
        cxx_readnetcdfHarmonicsElevation.cpp
        cxx_readnetcdfHarmonicsVelocity.cpp
        cxx_checkmesh.cpp
        cxx_read2dm.cpp
        cxx_kdtree.cpp
        cxx_writeasciifull.cpp
        cxx_writeasciisparse.cpp
        cxx_writeasciifullvector.cpp
        cxx_writeasciisparsevector.cpp
        cxx_writenetcdf.cpp
        cxx_writenetcdfvector.cpp
        cxx_writehdf5.cpp
        cxx_makemesh.cpp
        cxx_date.cpp)

    if(ENABLE_GDAL)
      set(TEST_LIST
          ${TEST_LIST} cxx_interpolateRaster.cpp cxx_interpolateManning.cpp
          cxx_interpolateDwind.cpp cxx_writeraster.cpp)
    endif(ENABLE_GDAL)

    if(OpenSSL_FOUND)
      set(TEST_LIST ${TEST_LIST} cxx_hash.cpp cxx_hashmesh.cpp)
    endif(OpenSSL_FOUND)

    foreach(TESTFILE ${TEST_LIST})
      get_filename_component(TESTNAME ${TESTFILE} NAME_WE)
      add_executable(${TESTNAME}
                     ${CMAKE_SOURCE_DIR}/testing/cxx_tests/${TESTFILE})
      add_dependencies(${TESTNAME} adcircmodules)
      target_include_directories(
        ${TESTNAME} PRIVATE ${CMAKE_SOURCE_DIR}/src
                            ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src)
      target_link_libraries(${TESTNAME} adcircmodules)
      set_target_properties(
        ${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                               ${CMAKE_BINARY_DIR}/cxx_testcases)

      add_test(
        NAME TEST_${TESTNAME}
        COMMAND ${CMAKE_BINARY_DIR}/cxx_testcases/${TESTNAME}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing)
      if(CYGWIN)
        set_tests_properties(
          TEST_${TESTNAME} PROPERTIES ENVIRONMENT
                                      "PATH=$ENV{PATH}:${CMAKE_BINARY_DIR}")
      else(CYGWIN)
        set_tests_properties(
          TEST_${TESTNAME}
          PROPERTIES ENVIRONMENT
                     "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}"
        )
      endif(CYGWIN)
    endforeach()

    # ...Script base testing
    add_test(
      NAME sh_interpStations
      COMMAND bash interpolate.sh
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing/test_scripts)
    set_property(
      TEST sh_interpStations
      PROPERTY ENVIRONMENT
               "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}")

    # ...Python Test
    if(PYTHON_TESTING)
      if(PYTHONTYPE EQUAL 2)
        set(PYTHON_TESTNAME TEST_python2_PyAdcirc)
        add_test(
          NAME ${PYTHON_TESTNAME}
          COMMAND
            ${PYTHON_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/testing/python_tests/pyadcirc_python2_testcase.py
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
      else()
        set(PYTHON_TESTNAME TEST_python3_PyAdcirc)
        add_test(
          NAME ${PYTHON_TESTNAME}
          COMMAND
            ${PYTHON_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/testing/python_tests/pyadcirc_python3_testcase.py
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
      endif()
      set_property(
        TEST ${PYTHON_TESTNAME}
        PROPERTY ENVIRONMENT
                 "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}")
      set_property(
        TEST ${PYTHON_TESTNAME}
        APPEND
        PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}")
    endif(PYTHON_TESTING)
  endif(BUILD_TESTS)
endif(UNIX OR CYGWIN)
# ##############################################################################

# ##############################################################################
# Utilities
# ##############################################################################
add_executable(interpolateAdcircStations
               ${CMAKE_SOURCE_DIR}/utilities/interpolateAdcircStations/main.cpp)
target_include_directories(
  interpolateAdcircStations
  PRIVATE ${CMAKE_SOURCE_DIR}/utilities/interpolateAdcircStations
          ${Boost_INCLUDE_DIRS}
          ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
          ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src)
add_dependencies(interpolateAdcircStations adcircmodules ezproj)
target_link_libraries(interpolateAdcircStations PRIVATE adcircmodules ezproj
                                                        ${Boost_LIBRARIES})
install(TARGETS interpolateAdcircStations RUNTIME DESTINATION bin)

add_executable(
  generateAdcircSimulationDates
  ${CMAKE_SOURCE_DIR}/utilities/generateAdcircSimulationDates/main.cpp)
target_include_directories(
  generateAdcircSimulationDates
  PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
          ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src)
add_dependencies(generateAdcircSimulationDates adcircmodules ezproj)
target_link_libraries(generateAdcircSimulationDates
                      PRIVATE adcircmodules ezproj ${Boost_LIBRARIES})
install(TARGETS generateAdcircSimulationDates RUNTIME DESTINATION bin)

add_executable(resultsScope ${CMAKE_SOURCE_DIR}/utilities/resultsScope/main.cpp)
target_include_directories(
  resultsScope
  PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src
          ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
          ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src)
add_dependencies(resultsScope adcircmodules ezproj)
target_link_libraries(resultsScope PRIVATE adcircmodules ezproj
                                           ${Boost_LIBRARIES})
install(TARGETS resultsScope RUNTIME DESTINATION bin)
# ##############################################################################
