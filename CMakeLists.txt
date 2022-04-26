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
if(APPLE)
  cmake_minimum_required(VERSION 3.12.0)
else()
  cmake_minimum_required(VERSION 3.0.0)
endif()
project(ADCIRCModules)
enable_language(C)
enable_language(CXX)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# ##############################################################################

# ##############################################################################
# Enable running tests
# ##############################################################################
if(UNIX OR CYGWIN)
  enable_testing()
endif(UNIX OR CYGWIN)
# ##############################################################################

# ##############################################################################
# Fast math option
# ##############################################################################
option(ADCIRCMODULES_USE_FAST_MATH
       "Use approximations to slow math ops where appropriate" OFF)
# ##############################################################################

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/coverage.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/cxxstandard.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/python.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/netcdf.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/hdf5.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/openmp.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/openssl.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gdal.cmake)

# ##############################################################################
# Compiler flags
# ##############################################################################
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_RELEASE)
mark_as_advanced(CLEAR CMAKE_CXX_FLAGS_DEBUG)
mark_as_advanced(CLEAR CMAKE_C_FLAGS_RELEASE)
mark_as_advanced(CLEAR CMAKE_C_FLAGS_DEBUG)
mark_as_advanced(CLEAR CMAKE_CXX_COMPILER)
mark_as_advanced(CLEAR CMAKE_C_COMPILER)
# ##############################################################################

# ##############################################################################
# TESTING
# ##############################################################################
option(BUILD_TESTS "Build test cases" OFF)
# ##############################################################################

# ##############################################################################
# DEBUGGING
# ##############################################################################
option(ADCIRCMODULES_ENABLE_STACK_TRACE
       "Enable a stack trace when used with address sanitizer" OFF)
mark_as_advanced(ADCIRCMODULES_ENABLE_STACK_TRACE)
# ##############################################################################

# ##############################################################################
# SET THE LOCATION OF TEMPORARY STATIC LIBS
# ##############################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles)
# ##############################################################################

# ##############################################################################
# LIBRARIES
# ##############################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/proj.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/shapelib.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/libadcircmodules.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/python_adcircmodules.cmake)
# ##############################################################################

# ##############################################################################
# UTILITIES
# ##############################################################################
include(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/generateAdcircSimulationDates.cmake)
include(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/interpolateAdcircStations.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resultsScope.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/highWaterMarks.cmake)
# ##############################################################################

# ##############################################################################
# TESTING
# ##############################################################################
if(BUILD_TESTS)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/test_cases.cmake)
endif()
# ##############################################################################
