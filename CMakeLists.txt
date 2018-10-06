
###########################################################################
#                     CMake Build File for ADCIRCMODULES
#
#    Written By: Zach Cobell
#
###########################################################################
#
# The CMake build system enable ADCIRCModules to be deployed and built
# in a cross platform environment. 
#
###########################################################################
INCLUDE (CheckIncludeFiles)
INCLUDE (CheckLibraryExists) 
INCLUDE (CheckFunctionExists)

#...Set the default build type
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
        build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
        Release RelWithDebInfo MinSizeRel.")
ELSEIF(COVERAGE)
        SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build,
            options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
            RelWithDebInfo MinSizeRel.")
ELSE()
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build,
        options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release
        RelWithDebInfo MinSizeRel.")
ENDIF()

###########################################################################
#  GENERAL OPTIONS
###########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)
PROJECT(ADCIRCModules)
###########################################################################


###########################################################################
# Enable running tests
###########################################################################
IF(UNIX OR CYGWIN)
    ENABLE_TESTING()
ENDIF(UNIX OR CYGWIN)
###########################################################################


###########################################################################
# Enable Coverage
###########################################################################
#OPTION(COVERAGE "Export Code Coverage report from tests" OFF)
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMakeModules)
IF(COVERAGE)
    IF(CMAKE_COMPILER_IS_GNUCXX) 
        INCLUDE(CodeCoverage)
        setup_target_for_coverage(adcmoduels_coverage tests coverage)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -g -O0 -fprofile-arcs -ftest-coverage")
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(COVERAGE)
###########################################################################


###########################################################################
# C++ 11 Check
###########################################################################
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler.")
endif()
###########################################################################


###########################################################################
#  Compiler flags 
###########################################################################
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
MARK_AS_ADVANCED( CLEAR CMAKE_CXX_FLAGS_RELEASE )
MARK_AS_ADVANCED( CLEAR CMAKE_CXX_FLAGS_DEBUG )
MARK_AS_ADVANCED( CLEAR CMAKE_C_FLAGS_RELEASE )
MARK_AS_ADVANCED( CLEAR CMAKE_C_FLAGS_DEBUG )
MARK_AS_ADVANCED( CLEAR CMAKE_CXX_COMPILER )
MARK_AS_ADVANCED( CLEAR CMAKE_C_COMPILER )
###########################################################################

###########################################################################
# CODE VERSION (GIT)
###########################################################################
EXECUTE_PROCESS( COMMAND git describe --always --tags
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_VERSION
                    OUTPUT_STRIP_TRAILING_WHITESPACE )
###########################################################################

###########################################################################
#  LIBRARY VERSION
###########################################################################
set(ADCIRCMODULES_VERSION_MAJOR 0)
set(ADCIRCMODULES_VERSION_MINOR 1)
set(ADCIRCMODULES_VERSION_PATCH 0)
set(ADCIRCMODULES_VERSION_STRING ${ADCIRCMODULES_VERSION_MAJOR}.${ADCIRCMODULES_VERSION_MINOR}.${ADCIRCMODULES_VERSION_PATCH})
###########################################################################

###########################################################################
# TESTING 
###########################################################################
OPTION(BUILD_TESTS "Build test cases" OFF)
###########################################################################

###########################################################################
#  SET THE LOCATION OF TEMPORARY STATIC LIBS
###########################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles)
###########################################################################


###########################################################################
#  SWIG/PYTHON/PERL
###########################################################################
FIND_PACKAGE(SWIG 3.0)
IF(SWIG_FOUND)
    INCLUDE(${SWIG_USE_FILE})

    FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
    IF(PythonLibs_FOUND)
        FIND_PACKAGE(PythonInterp 2.7 REQUIRED)
        INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
        OPTION(PYTHON_TESTING "Test the Python Libraries with CTest" OFF)
        EXECUTE_PROCESS( COMMAND ${PYTHON_EXECUTABLE} -m site --user-site 
                         OUTPUT_VARIABLE temp_PYTHON_SITE_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
        SET( PYTHON_INSTALL_DIRECTORY ${temp_PYTHON_SITE_DIRECTORY} CACHE STRING "Location where Python libraries will be installed" )
        MARK_AS_ADVANCED( CLEAR PYTHON_EXECUTABLE )
    ELSE(PythonLibs_FOUND)
        SET( PYTHON_INSTALL_DIRECTORY "PYTHON_INSTALL_DIRECTORY-NOTFOUND" CACHE STRING "Location where Python libraries will be installed" )
    ENDIF(PythonLibs_FOUND)
    
    FIND_PACKAGE(PerlLibs)
    IF(PerlLibs_FOUND)
        OPTION(TEST_PERL "Test the perl build for SWIG. Note: Does not work yet" OFF) 
    ENDIF(PerlLibs_FOUND)

ENDIF(SWIG_FOUND)
###########################################################################
    
    
###########################################################################
#  NETCDF
###########################################################################
IF(NOT "${NETCDFHOME}" STREQUAL "")
    SET(NETCDF_DIR "${NETCDFHOME}")
ELSEIF(NOT $ENV{NETCDFHOME} STREQUAL "")
    SET(NETCDF_DIR $ENV{NETCDFHOME})
ENDIF(NOT "${NETCDFHOME}" STREQUAL "")

FIND_PACKAGE(NetCDF)

SET(NETCDF_AdditionalLibs "" CACHE STRING "Additional libraries that may be required for netCDF")

IF(NOT NETCDF_FOUND)
    MESSAGE(SEND_ERROR "Specify the netCDF path on the following screen")
ELSE(NOT NETCDF_FOUND)
    
    SET(netcdf_c_code
"
    #include <netcdf.h>
    int main(){
        int ncid,varid;
        int ierr = nc_def_var_deflate(ncid,varid,1,2,2);
        return 0;
    }
"
    )
    FILE(WRITE "${CMAKE_BINARY_DIR}/CMakeFiles/netcdf_c_check.c" "${netcdf_c_code}")
    TRY_COMPILE(NC_DEFLATE_FOUND "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/netcdf_c_check.c" CMAKE_FLAGS "-DINCLUDE_DIRECTORIES=${NETCDF_INCLUDE_DIRS}" LINK_LIBRARIES "${NETCDF_LIBRARIES}" LINK_LIBRARIES "${NETCDF_AdditionalLibs}" OUTPUT_VARIABLE LOG1)
        
    IF(NC_DEFLATE_FOUND)
        SET(NETCDF_LINKER_FLAG "${NETCDF_LIBRARIES}")
    ELSE(NC_DEFLATE_FOUND)
        MESSAGE(SEND_ERROR "The netCDF library is not functional.")
    ENDIF(NC_DEFLATE_FOUND)
ENDIF(NOT NETCDF_FOUND)
###########################################################################


###########################################################################
# OpenMP
###########################################################################
OPTION(ENABLE_OPENMP "Enable OpenMP Multithreading" ON)
IF(ENABLE_OPENMP)
    Find_Package(OpenMP)
ELSE(ENABLE_OPENMP)

ENDIF(ENABLE_OPENMP)
###########################################################################


###########################################################################
# GDAL
###########################################################################
OPTION(ENABLE_GDAL "Enabele GDAL" ON )
IF(ENABLE_GDAL)
    Find_PACKAGE(GDAL)
    IF(NOT GDAL_FOUND)
        MESSAGE( WARNING "GDAL Library not found. These functions will be disabled.")
    ENDIF(NOT GDAL_FOUND)
ELSE(ENABLE_GDAL)
    MESSAGE( WARNING "GDAL Library has been disabled. Raster operations will not be available.")
    SET(GDAL_FOUND FALSE)
    SET(ENABLE_GDAL OFF)
ENDIF(ENABLE_GDAL)
###########################################################################

###########################################################################
# PROJ4 Configuration Options
###########################################################################
#...C options, lifted from Proj4
IF (CMAKE_C_COMPILER_ID STREQUAL "Intel")
  IF (MSVC)
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /fp:precise")
  ELSE ()
    SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fp-model precise")
  ENDIF ()
ENDIF ()
SET(CMAKE_THREAD_PREFER_PTHREAD TRUE)
FIND_PACKAGE (Threads)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckSymbolExists)
CHECK_SYMBOL_EXISTS(PTHREAD_MUTEX_RECURSIVE pthread.h HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)

INCLUDE (CheckCSourceCompiles)
IF (MSVC)
  SET (CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS} /WX")
ELSE ()
  SET (CMAKE_REQUIRED_LIBRARIES m)
  SET (CMAKE_REQUIRED_FLAGS "${CMAKE_C_FLAGS} -Werror")
ENDIF ()
# Check whether the C99 math function: hypot, atanh, etc. are available.
check_c_source_compiles (
  "#include <math.h>
int main() {
  int q;
  return (int)(hypot(3.0, 4.0) + atanh(0.8) + cbrt(8.0) +
               remquo(100.0, 90.0, &q) +
               remainder(100.0, 90.0) + copysign(1.0, -0.0) +
               log1p(0.1) + asinh(0.1)) +
               isnan(0.0);
}\n" C99_MATH)

# check needed include file
check_include_files (dlfcn.h HAVE_DLFCN_H)
check_include_files (inttypes.h HAVE_INTTYPES_H)
check_include_files (jni.h HAVE_JNI_H)
check_include_files (memory.h HAVE_MEMORY_H)
check_include_files (stdint.h HAVE_STDINT_H)
check_include_files (stdlib.h HAVE_STDLIB_H)
check_include_files (string.h HAVE_STRING_H)
check_include_files (sys/stat.h HAVE_SYS_STAT_H)
check_include_files (sys/types.h HAVE_SYS_TYPES_H)
check_include_files (unistd.h HAVE_UNISTD_H)
check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

CHECK_FUNCTION_EXISTS(localeconv HAVE_LOCALECONV)

# check libm need on unix 
check_library_exists(m ceil "" HAVE_LIBM) 

set(PACKAGE "proj")
set(PACKAGE_BUGREPORT "https://github.com/OSGeo/proj.4/issues")
set(PACKAGE_NAME "PROJ")
set(PACKAGE_STRING "PROJ ${${PROJECT_INTERN_NAME}_VERSION}")
set(PACKAGE_TARNAME "proj")
set(PACKAGE_URL "http://proj4.org")
set(PACKAGE_VERSION "${${PROJECT_INTERN_NAME}_VERSION}")

configure_file(${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/proj_config.cmake.in ${CMAKE_BINARY_DIR}/CMakeFiles/pj_config/proj_config.h)
###########################################################################


###########################################################################
###########################################################################
#  LIBRARIES    
###########################################################################
###########################################################################


###########################################################################
#  POINT Library
###########################################################################
ADD_LIBRARY( point STATIC ${CMAKE_SOURCE_DIR}/libraries/point/point.cpp )
TARGET_INCLUDE_DIRECTORIES( point PRIVATE ${CMAKE_SOURCE_DIR}/libraries/point )
###########################################################################


###########################################################################
# Rasterdata
###########################################################################
IF(GDAL_FOUND)
    ADD_LIBRARY( rasterdata STATIC ${CMAKE_SOURCE_DIR}/libraries/rasterdata/rasterdata.cpp 
                                   ${CMAKE_SOURCE_DIR}/libraries/rasterdata/pixel.cpp )
    TARGET_INCLUDE_DIRECTORIES( rasterdata PRIVATE ${CMAKE_SOURCE_DIR}/libraries/rasterdata )
    TARGET_INCLUDE_DIRECTORIES( rasterdata PRIVATE ${GDAL_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/libraries/point )
    TARGET_LINK_LIBRARIES( rasterdata point )
ENDIF(GDAL_FOUND)
###########################################################################


###########################################################################
#  KDTREE2 Library
###########################################################################
ADD_LIBRARY( kdtree2 STATIC ${CMAKE_SOURCE_DIR}/libraries/kdtree2/kdtree2lib.cpp 
                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree/kdtree2.cpp )
TARGET_INCLUDE_DIRECTORIES( kdtree2 PRIVATE ${CMAKE_SOURCE_DIR}/libraries/kdtree2
                                ${CMAKE_SOURCE_DIR}/libraries/point
                                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree
                                ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_66_0 )
ADD_DEPENDENCIES( kdtree2 point)
TARGET_LINK_LIBRARIES( kdtree2 point )
###########################################################################


###########################################################################
#  PROJ4 Library
###########################################################################
ADD_LIBRARY( projection STATIC ${CMAKE_SOURCE_DIR}/libraries/projection/projection.cpp
                          ${CMAKE_SOURCE_DIR}/libraries/projection/epsg.cpp
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/nad_init.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_aea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_aeqd.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_airy.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_aitoff.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_august.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_axisswap.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_bacon.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_bipc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_boggs.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_bonne.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_calcofi.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_cart.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_cass.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_cc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_ccon.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_cea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_chamb.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_collg.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_comill.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_crast.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_deformation.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_denoy.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_eck1.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_eck2.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_eck3.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_eck4.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_eck5.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_eqc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_eqdc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_fahey.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_fouc_s.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_gall.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_geoc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_geos.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_gins8.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_gnom.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_gn_sinu.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_goode.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_gstmerc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_hammer.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_hatano.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_helmert.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_hgridshift.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_horner.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_igh.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_isea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_imw_p.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_krovak.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_labrd.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_laea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_lagrng.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_larr.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_lask.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_latlong.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_lcca.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_lcc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_loxim.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_lsat.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_misrsom.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_mbt_fps.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_mbtfpp.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_mbtfpq.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_merc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_mill.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_mod_ster.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_moll.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_molodensky.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_natearth.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_natearth2.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_nell.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_nell_h.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_nocol.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_nsper.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_nzmg.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_ob_tran.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_ocea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_oea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_omerc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_ortho.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_patterson.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_pipeline.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_poly.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_putp2.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_putp3.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_putp4p.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_putp5.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_putp6.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_qsc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_robin.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_rpoly.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_sch.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_sconics.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_somerc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_sterea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_stere.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_sts.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_tcc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_tcea.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_times.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_tmerc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_tpeqd.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_unitconvert.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_urm5.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_urmfps.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_vandg.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_vandg2.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_vandg4.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_vgridshift.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_wag2.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_wag3.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_wag7.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_wink1.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_wink2.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/proj_etmerc.c 
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/aasincos.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/adjlon.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/bch2bps.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/bchgen.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/biveval.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/dmstor.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/emess.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/emess.h
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/geocent.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/geocent.h
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/geodesic.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/mk_cheby.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/nad_cvt.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/nad_init.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/nad_intr.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_apply_gridshift.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_apply_vgridshift.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_auth.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_ctx.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_fileapi.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_datum_set.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_datums.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_deriv.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_ell_set.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_ellps.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_errno.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_factors.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_fwd.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_gauss.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_gc_reader.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_geocent.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_gridcatalog.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_gridinfo.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_gridlist.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/PJ_healpix.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_init.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_initcache.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_inv.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_list.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_list.h
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_log.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_malloc.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_math.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_mlfn.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_msfn.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_mutex.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/proj_4D_api.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_internal.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/proj_internal.h
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_open_lib.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_param.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_phi2.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_pr_list.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_qsfn.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_release.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_strerrno.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_transform.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_tsfn.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_units.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_utils.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_zpoly1.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/proj_mdist.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/proj_math.h
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/proj_rouss.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/rtodms.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/vector1.c
                          ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src/pj_strtod.c )

TARGET_INCLUDE_DIRECTORIES( projection PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/proj5.1/src
                                ${CMAKE_SOURCE_DIR}/libraries/point 
                                ${CMAKE_SOURCE_DIR}/libraries/constants
                                ${CMAKE_SOURCE_DIR}/libraries/projection
                                ${CMAKE_BINARY_DIR}/CMakeFiles/pj_config )

TARGET_LINK_LIBRARIES( projection point constants )
ADD_DEPENDENCIES( projection point constants )
IF (C99_MATH)
  TARGET_COMPILE_DEFINITIONS (projection PRIVATE "HAVE_C99_MATH=1")
ELSE ()
  TARGET_COMPILE_DEFINITIONS (projection PRIVATE "HAVE_C99_MATH=0")
ENDIF ()
IF (HAVE_PTHREAD_MUTEX_RECURSIVE_DEFN)
    TARGET_COMPILE_DEFINITIONS(projection PRIVATE "HAVE_PTHREAD_MUTEX_RECURSIVE=1")
ENDIF()
###########################################################################

###########################################################################
#  Constants Library
###########################################################################
ADD_LIBRARY( constants STATIC ${CMAKE_SOURCE_DIR}/libraries/constants/constants.cpp )
TARGET_INCLUDE_DIRECTORIES( constants PRIVATE ${CMAKE_SOURCE_DIR}/libraries/constants )
###########################################################################


###########################################################################
#  Shapelib Library
###########################################################################
ADD_LIBRARY( shapelib STATIC ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/shpopen.c
                ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/dbfopen.c
                ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/safileio.c
                ${CMAKE_SOURCE_DIR}/thirdparty/shapelib/shptree.c )
TARGET_INCLUDE_DIRECTORIES( shapelib PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/shapelib )
###########################################################################


###########################################################################
#  ADCIRCModules Library
###########################################################################
SET( ADCIRCMODULES_SOURCES 
                ${CMAKE_SOURCE_DIR}/src/config.cpp
                ${CMAKE_SOURCE_DIR}/src/error.cpp
                ${CMAKE_SOURCE_DIR}/src/filetypes.cpp
                ${CMAKE_SOURCE_DIR}/src/mesh.cpp 
                ${CMAKE_SOURCE_DIR}/src/node.cpp
                ${CMAKE_SOURCE_DIR}/src/element.cpp
                ${CMAKE_SOURCE_DIR}/src/boundary.cpp
                ${CMAKE_SOURCE_DIR}/src/split.cpp
                ${CMAKE_SOURCE_DIR}/src/io.cpp
                ${CMAKE_SOURCE_DIR}/src/stringconversion.cpp
                ${CMAKE_SOURCE_DIR}/src/attribute.cpp
                ${CMAKE_SOURCE_DIR}/src/attributemetadata.cpp
                ${CMAKE_SOURCE_DIR}/src/nodalattributes.cpp 
                ${CMAKE_SOURCE_DIR}/src/outputrecord.cpp 
                ${CMAKE_SOURCE_DIR}/src/outputfile.cpp 
                ${CMAKE_SOURCE_DIR}/src/harmonicsrecord.cpp
                ${CMAKE_SOURCE_DIR}/src/harmonicsoutput.cpp 
                ${CMAKE_SOURCE_DIR}/src/elementtable.cpp
                ${CMAKE_SOURCE_DIR}/src/meshchecker.cpp 
                ${CMAKE_SOURCE_DIR}/src/multithreading.cpp )

IF( GDAL_FOUND )
    SET( ADCIRCMODULES_SOURCES ${ADCIRCMODULES_SOURCES} ${CMAKE_SOURCE_DIR}/src/griddata.cpp )
ENDIF( GDAL_FOUND )

ADD_LIBRARY( adcircmodules SHARED ${ADCIRCMODULES_SOURCES} )

TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_SOURCE_DIR}/libraries/point
                                ${CMAKE_SOURCE_DIR}/libraries/kdtree2
                                ${CMAKE_SOURCE_DIR}/libraries/projection
                                ${CMAKE_SOURCE_DIR}/thirdparty/shapelib
                                ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_66_0 
                                ${CMAKE_SOURCE_DIR}/libraries/constants
                                ${NETCDF_INCLUDE_DIR} ) 

ADD_DEPENDENCIES( adcircmodules point projection kdtree2 shapelib constants )

TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE GIT_VERSION="${GIT_VERSION}")

IF(GDAL_FOUND)
    TARGET_LINK_LIBRARIES( adcircmodules rasterdata ${GDAL_LIBRARY} ) 
    TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE  
                                ${CMAKE_SOURCE_DIR}/libraries/rasterdata )
    ADD_DEPENDENCIES( adcircmodules rasterdata )
ENDIF(GDAL_FOUND)

SET_TARGET_PROPERTIES( adcircmodules PROPERTIES VERSION ${ADCIRCMODULES_VERSION_STRING} SOVERSION ${ADCIRCMODULES_VERSION_MAJOR} )

SET(HEADER_LIST ${CMAKE_SOURCE_DIR}/src/adcirc.h
                ${CMAKE_SOURCE_DIR}/src/attribute.h
                ${CMAKE_SOURCE_DIR}/src/attributemetadata.h
                ${CMAKE_SOURCE_DIR}/src/boundary.h
                ${CMAKE_SOURCE_DIR}/src/config.h
                ${CMAKE_SOURCE_DIR}/src/element.h
                ${CMAKE_SOURCE_DIR}/src/error.h
                ${CMAKE_SOURCE_DIR}/src/harmonicsoutput.h
                ${CMAKE_SOURCE_DIR}/src/harmonicsrecord.h
                ${CMAKE_SOURCE_DIR}/src/io.h
                ${CMAKE_SOURCE_DIR}/src/mesh.h
                ${CMAKE_SOURCE_DIR}/src/nodalattributes.h
                ${CMAKE_SOURCE_DIR}/src/node.h
                ${CMAKE_SOURCE_DIR}/src/outputfile.h
                ${CMAKE_SOURCE_DIR}/src/outputrecord.h
                ${CMAKE_SOURCE_DIR}/src/meshchecker.h
                ${CMAKE_SOURCE_DIR}/src/elementtable.h
                ${CMAKE_SOURCE_DIR}/libraries/point/point.h 
                ${CMAKE_SOURCE_DIR}/libraries/kdtree2/kdtree2lib.h 
                ${CMAKE_SOURCE_DIR}/libraries/projection/projection.h 
                ${CMAKE_SOURCE_DIR}/libraries/rasterdata/rasterdata.h 
                ${CMAKE_SOURCE_DIR}/src/griddata.h 
                ${CMAKE_SOURCE_DIR}/src/multithreading.h ) 

SET_TARGET_PROPERTIES( adcircmodules PROPERTIES PUBLIC_HEADER "${HEADER_LIST}" ) 

IF(${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local" OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local/" OR
   ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/" OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/")
    SET(HEADER_DEST "include/adcirc")
ELSE()
    SET(HEADER_DEST "include")
ENDIF()

IF(WIN32)
    INSTALL(TARGETS adcircmodules LIBRARY DESTINATION bin RUNTIME DESTINATION lib PUBLIC_HEADER DESTINATION ${HEADER_DEST})
ELSEIF(CYGWIN)
    INSTALL(TARGETS adcircmodules LIBRARY DESTINATION lib RUNTIME DESTINATION lib PUBLIC_HEADER DESTINATION ${HEADER_DEST})
ELSE(WIN32)
    INSTALL( TARGETS adcircmodules LIBRARY DESTINATION lib PUBLIC_HEADER DESTINATION ${HEADER_DEST})
ENDIF(WIN32)

TARGET_LINK_LIBRARIES( adcircmodules projection point kdtree2 shapelib constants ${NETCDF_LIBRARIES} )
IF(OPENMP_FOUND)
    TARGET_COMPILE_OPTIONS( adcircmodules PRIVATE ${OpenMP_CXX_FLAGS})
    TARGET_LINK_LIBRARIES(  adcircmodules ${OpenMP_CXX_LIB_NAMES} ${OpenMP_CXX_FLAGS} )
ENDIF(OPENMP_FOUND)


###########################################################################


###########################################################################
# Test case
###########################################################################
###########################################################################

###########################################################################
#  PyAdcirc Library
###########################################################################
IF(PythonLibs_FOUND AND SWIG_FOUND)

    IF(GDAL_FOUND)
        SET(SWIG_GDAL_FLAG "-D_USE_GDAL")
    ENDIF(GDAL_FOUND)
    
    IF(WIN32)

        #...Builds a dummy cmake project
        #   Which will add some windows specific include
        #   files not required by Unix/Cygwin [mingw64]
        #   Further testing required to see if this is 
        #   required by MSVC compilers
        FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeFiles/python_wrapper.cmake "
            FILE(READ \${SRC} DATA)
            FILE(WRITE \${DST} \"#include <math.h>\")
            FILE(APPEND \${DST} \"\\n\\n\")
            FILE(APPEND \${DST} \"\${DATA}\")
        ")

        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python -I${CMAKE_SOURCE_DIR}/src 
                    -I${CMAKE_SOURCE_DIR}/libraries/point -I${CMAKE_SOURCE_DIR}/libraries/kdtree2 
                    -I${CMAKE_SOURCE_DIR}/libraries/projection -I${CMAKE_SOURCE_DIR}/libraries/rasterdata
                    -I${CMAKE_SOURCE_DIR}/libraries/constants -I${PYTHON_INCLUDE_PATH} ${SWIG_GDAL_FLAG}
                    -o ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1
                    ${CMAKE_SOURCE_DIR}/swig/adcirc.i
            COMMAND ${CMAKE_COMMAND} -DSRC=\"${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1\" 
                                     -DDST=\"${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx\"
                                     -P ${CMAKE_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python..."
        )

    ELSE(WIN32)
        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python -I${CMAKE_SOURCE_DIR}/src 
                    -I${CMAKE_SOURCE_DIR}/libraries/point -I${CMAKE_SOURCE_DIR}/libraries/kdtree2
                    -I${CMAKE_SOURCE_DIR}/libraries/projection -I${CMAKE_SOURCE_DIR}/libraries/rasterdata
                    -I${CMAKE_SOURCE_DIR}/libraries/constants -I${PYTHON_INCLUDE_PATH} ${SWIG_GDAL_FLAG} 
                    -o ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
                    ${CMAKE_SOURCE_DIR}/swig/adcirc.i
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python..."
        )
    ENDIF(WIN32)

    ADD_LIBRARY( PyAdcirc SHARED ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx )
    TARGET_INCLUDE_DIRECTORIES( PyAdcirc PRIVATE ${PYTHON_INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR}/src 
                                    ${CMAKE_SOURCE_DIR}/libraries/point ${CMAKE_SOURCE_DIR}/libraries/kdtree2
                                    ${CMAKE_SOURCE_DIR}/libraries/projection ${CMAKE_SOURCE_DIR}/libraries/constants 
                                    ${CMAKE_SOURCE_DIR}/libraries/rasterdata )
    TARGET_LINK_LIBRARIES( PyAdcirc PRIVATE adcircmodules ${PYTHON_LIBRARY} )
    SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES PREFIX "_")
    SET_PROPERTY( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES PyAdcirc.py CMakeFiles/python_adcirc_wrap.cxx )

    ADD_DEPENDENCIES( PyAdcirc adcircmodules )

    IF(WIN32)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
                                                   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
        INSTALL(TARGETS PyAdcirc LIBRARY DESTINATION bin RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
    ELSEIF(CYGWIN)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
        INSTALL(TARGETS PyAdcirc LIBRARY DESTINATION lib RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
    ELSE(WIN32)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
        INSTALL(TARGETS PyAdcirc LIBRARY DESTINATION ${PYTHON_INSTALL_DIRECTORY})
    ENDIF(WIN32)
    
    INSTALL(FILES ${CMAKE_BINARY_DIR}/PyAdcirc.py DESTINATION ${PYTHON_INSTALL_DIRECTORY})
    
    IF(WIN32)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES SUFFIX ".pyd")
        IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
            # 64 bits
            TARGET_COMPILE_DEFINITIONS(PyAdcirc PRIVATE "MS_WIN64")
        ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
            # 32 bits
            TARGET_COMPILE_DEFINITIONS(PyAdcirc PRIVATE "MS_WIN32")
        ENDIF()
    ENDIF(WIN32)
ENDIF()
###########################################################################

###########################################################################
#  PerlAdcirc Library
###########################################################################
IF(TEST_PERL)
    IF(Perl_FOUND AND SWIG_FOUND)
        IF(WIN32)
    
            #...Builds a dummy cmake project
            #   Which will add some windows specific include
            #   files not required by Unix/Cygwin [mingw64]
            #   Further testing required to see if this is 
            #   required by MSVC compilers
            FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeFiles/perlwrapper.cmake "
                FILE(READ \${SRC} DATA)
                FILE(WRITE \${DST} \"#include <math.h>\")
                FILE(APPEND \${DST} \"\\n\\n\")
                FILE(APPEND \${DST} \"\${DATA}\")
            ")
    
            ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
                COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -perl -I${CMAKE_SOURCE_DIR}/src
                        -I${CMAKE_SOURCE_DIR}/libraries/point -I${CMAKE_SOURCE_DIR}/libraries/kdtree2 
                        -I${CMAKE_SOURCE_DIR}/libraries/projection -I${PERL_INCLUDE_PATH} 
                        -o ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx.1
                        ${CMAKE_SOURCE_DIR}/swig/adcirc.i
                COMMAND ${CMAKE_COMMAND} -DSRC=${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx.1 
                                         -DDST=${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
                                         -P ${CMAKE_BINARY_DIR}/CMakeFiles/perl_wrapper.cmake
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating SWIG interface to Perl..."
            )
    
        ELSE(WIN32)
            ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
                COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -perl -I${CMAKE_SOURCE_DIR}/src 
                        -I${CMAKE_SOURCE_DIR}/libraries/point -I${CMAKE_SOURCE_DIR}/libraries/kdtree2
                        -I${CMAKE_SOURCE_DIR}/libraries/projection -I${PERL_INCLUDE_PATH} 
                        -o ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
                        ${CMAKE_SOURCE_DIR}/swig/adcirc.i
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating SWIG interface to Perl..."
            )
        ENDIF(WIN32)
    
        ADD_LIBRARY( perladcirc SHARED ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx )
        TARGET_INCLUDE_DIRECTORIES( perladcirc PRIVATE ${PERL_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src 
                                        ${CMAKE_SOURCE_DIR}/libraries/point 
                                        ${CMAKE_SOURCE_DIR}/libraries/kdtree2
                                        ${CMAKE_SOURCE_DIR}/libraries/projection 
                                        ${CMAKE_SOURCE_DIR}/libraries/rasterdata )
        TARGET_LINK_LIBRARIES( perladcirc PRIVATE adcircmodules ${PERL_LIBRARY} )
        SET_TARGET_PROPERTIES( perladcirc PROPERTIES PREFIX "")
        SET_PROPERTY( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES perladcirc.pm CMakeFiles/perl_adcirc_wrap.cxx )
        ADD_DEPENDENCIES( perladcirc adcircmodules )
    
        TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE HAS_BOOL)
        
        IF(WIN32)
            IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
                # 64 bits
                TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE "MS_WIN64")
            ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
                # 32 bits
                TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE "MS_WIN32")
            ENDIF()
        ENDIF(WIN32)
    ENDIF(Perl_FOUND AND SWIG_FOUND)
ENDIF(TEST_PERL)
###########################################################################


###########################################################################
# Test Cases
###########################################################################
IF(UNIX OR CYGWIN)
    IF(BUILD_TESTS)
        #...C++ Testingc
        FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cxx_testcases)

        SET(TEST_LIST cxx_readmesh.cpp cxx_writemesh.cpp cxx_writeshapefile.cpp
                      cxx_projectmesh.cpp cxx_nodalSearchTree.cpp cxx_readfort13_wmesh.cpp 
                      cxx_readfort13_womesh.cpp cxx_fort13findatt.cpp cxx_readasciifull.cpp
                      cxx_readasciisparse.cpp cxx_readmaxele.cpp cxx_readnetcdfmaxele.cpp
                      cxx_readasciivector.cpp cxx_readnetcdf.cpp cxx_readnetcdfvector.cpp
                      cxx_readHarmonicsElevation.cpp cxx_readHarmonicsVelocity.cpp 
                      cxx_readnetcdfHarmonicsElevation.cpp  cxx_readnetcdfHarmonicsVelocity.cpp
                      cxx_checkmesh.cpp cxx_interpolateRaster.cpp cxx_interpolateManning.cpp 
                      cxx_interpolateDwind.cpp )

        FOREACH(TESTFILE ${TEST_LIST} ) 
            GET_FILENAME_COMPONENT( TESTNAME ${TESTFILE} NAME_WE )
            ADD_EXECUTABLE( ${TESTNAME} ${CMAKE_SOURCE_DIR}/testing/cxx_tests/${TESTFILE} )
            ADD_DEPENDENCIES( ${TESTNAME} adcircmodules )
            TARGET_INCLUDE_DIRECTORIES( ${TESTNAME} PRIVATE ${CMAKE_SOURCE_DIR}/src 
                                                            ${CMAKE_SOURCE_DIR}/libraries/point 
                                                            ${CMAKE_SOURCE_DIR}/libraries/projection 
                                                            ${CMAKE_SOURCE_DIR}/libraries/kdtree2 
                                                            ${CMAKE_SOURCE_DIR}/libraries/constants 
                                                            ${CMAKE_SOURCE_DIR}/libraries/rasterdata )
            TARGET_LINK_LIBRARIES( ${TESTNAME} adcircmodules )
            SET_TARGET_PROPERTIES( ${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cxx_testcases )
    
            ADD_TEST( NAME TEST_${TESTNAME} COMMAND ${CMAKE_BINARY_DIR}/cxx_testcases/${TESTNAME}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing )
            IF(CYGWIN)
               SET_TESTS_PROPERTIES( TEST_${TESTNAME} PROPERTIES ENVIRONMENT "PATH=$ENV{PATH}:${CMAKE_BINARY_DIR}")
            ELSE(CYGWIN)
               SET_TESTS_PROPERTIES( TEST_${TESTNAME} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}")
            ENDIF(CYGWIN)
        ENDFOREACH()
        
        #...Python Test
        IF(PYTHON_TESTING)
            ADD_TEST( NAME Python_Test COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/pyadcirc_testcase.py
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
            SET_PROPERTY( TEST Python_Test PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}")
            SET_PROPERTY( TEST Python_Test APPEND PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}")
        ENDIF(PYTHON_TESTING)
    ENDIF(BUILD_TESTS)
ENDIF(UNIX OR CYGWIN)
###########################################################################
