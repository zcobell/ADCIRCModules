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
cmake_minimum_required(VERSION 3.0.0)
project(ADCIRCModules)
enable_language(C)
enable_language(CXX)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckFunctionExists)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
# ##############################################################################

# ##############################################################################
# Enable running tests
# ##############################################################################
if(UNIX OR CYGWIN)
  enable_testing()
endif(UNIX OR CYGWIN)
# ##############################################################################
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/coverage.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/cxxstandard.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/version.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/abseil.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/boost.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/python.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/netcdf.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/hdf5.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/openmp.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/openssl.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/gdal.cmake)


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
# SET THE LOCATION OF TEMPORARY STATIC LIBS
# ##############################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles)
# ##############################################################################

# ##############################################################################
# LIBRARIES
# ##############################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/proj.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/shapelib.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/libadcircmodules.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/python_adcircmodules.cmake)
# ##############################################################################

# ##############################################################################
# UTILITIES
# ##############################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/generateAdcircSimulationDates.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/interpolateAdcircStations.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/resultsScope.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/highWaterMarks.cmake)
# ##############################################################################


# ##############################################################################
# TESTING
# ##############################################################################
IF(BUILD_TESTS)
  include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules/test_cases.cmake)
ENDIF()
# ##############################################################################
