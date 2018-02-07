
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

#...Set the default build type
IF(DEFINED CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Choose the type of
        build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug
        Release RelWithDebInfo MinSizeRel.")
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
    enable_testing()
ENDIF(UNIX OR CYGWIN)
###########################################################################


###########################################################################
# C++ 11 Check
###########################################################################
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
###########################################################################


###########################################################################
#  Compiler flags 
###########################################################################
SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
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
#  SET THE LOCATION OF TEMPORARY STATIC LIBS
###########################################################################
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles)
###########################################################################


###########################################################################
#  SWIG/PYTHON/PERL
###########################################################################
FIND_PACKAGE(SWIG)
IF(SWIG_FOUND)
    INCLUDE(${SWIG_USE_FILE})
ENDIF(SWIG_FOUND)

FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
IF(PythonLibs_FOUND)
    FIND_PACKAGE(PythonInterp 2.7 REQUIRED)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
    OPTION(PYTHON_TESTING "Test the Python Libraries with CTest" OFF)
ENDIF(PythonLibs_FOUND)

FIND_PACKAGE(PerlLibs)
IF(PerlLibs_FOUND)
    OPTION(TEST_PERL "Test the perl build for SWIG. Note: Does not work yet" OFF) 
ENDIF(PerlLibs_FOUND)
###########################################################################


###########################################################################
###########################################################################
#  LIBRARIES    
###########################################################################
###########################################################################


###########################################################################
#  POINT Library
###########################################################################
ADD_LIBRARY( point STATIC ${CMAKE_SOURCE_DIR}/src/adcirc/point/point.cpp )
TARGET_INCLUDE_DIRECTORIES( point PRIVATE ${CMAKE_SOURCE_DIR}/src/adcirc/point )
###########################################################################


###########################################################################
#  KDTREE2 Library
###########################################################################
ADD_LIBRARY( kdtree2 STATIC ${CMAKE_SOURCE_DIR}/src/adcirc/qKdtree2/qkdtree2.cpp 
                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree/kdtree2.cpp )
TARGET_INCLUDE_DIRECTORIES( kdtree2 PRIVATE ${CMAKE_SOURCE_DIR}/src/adcirc/qKdtree2
                                ${CMAKE_SOURCE_DIR}/src/adcirc/point
                                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree
                                ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_66_0 )
ADD_DEPENDENCIES( kdtree2 point)
TARGET_LINK_LIBRARIES( kdtree2 point )
###########################################################################


###########################################################################
#  PROJ4 Library
###########################################################################
ADD_LIBRARY( proj4 STATIC ${CMAKE_SOURCE_DIR}/src/adcirc/qProj4/qproj4.cpp 
        ${CMAKE_SOURCE_DIR}/src/adcirc/qProj4/epsg.cpp 
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/aasincos.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/adjlon.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/bch2bps.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/bchgen.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/biveval.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/cs2cs.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/dmstor.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/emess.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/gen_cheb.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/geocent.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/geod.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/geod_interface.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/geod_set.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/geodesic.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/hypot.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/jniproj.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/mk_cheby.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/nad_cvt.c 
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/nad_init.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/nad_intr.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/nad2bin.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/p_series.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_aea.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_aeqd.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_airy.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_aitoff.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_apply_gridshift.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_apply_vgridshift.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_august.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_auth.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_bacon.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_bipc.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_boggs.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_bonne.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_calcofi.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_cass.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_cc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_cea.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_chamb.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_collg.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_crast.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_ctx.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_datum_set.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_datums.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_denoy.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_deriv.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_eck1.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_eck2.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_eck3.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_eck4.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_eck5.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_ell_set.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_ellps.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_eqc.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_eqdc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_errno.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_factors.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_fahey.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_fileapi.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_fouc_s.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_fwd.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_gall.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_gauss.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_gc_reader.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_geocent.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_geos.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_gins8.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_gn_sinu.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_gnom.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_goode.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_gridcatalog.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_gridinfo.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_gridlist.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_gstmerc.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_hammer.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_hatano.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_healpix.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_igh.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_imw_p.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_init.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_initcache.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_inv.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_isea.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_krovak.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_labrd.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_laea.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_lagrng.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_larr.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_lask.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_latlong.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_lcc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_lcca.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_list.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_log.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_loxim.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_lsat.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_malloc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_mbt_fps.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_mbtfpp.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_mbtfpq.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_merc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_mill.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_mlfn.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_mod_ster.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_moll.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_msfn.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_mutex.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_natearth.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_nell.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_nell_h.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_nocol.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_nsper.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_nzmg.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_ob_tran.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_ocea.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_oea.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_omerc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_open_lib.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_ortho.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_param.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_phi2.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_poly.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_pr_list.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_putp2.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_putp3.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_putp4p.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_putp5.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_putp6.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_qsc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_qsfn.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_release.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_robin.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_rpoly.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_sconics.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_somerc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_stere.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_sterea.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_strerrno.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_strtod.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_sts.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_tcc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_tcea.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_tmerc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_tpeqd.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_transform.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_tsfn.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_units.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_urm5.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_urmfps.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_utils.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_vandg.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_vandg2.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_vandg4.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_wag2.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_wag3.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_wag7.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_wink1.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/PJ_wink2.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/pj_zpoly1.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/proj.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/proj_etmerc.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/proj_mdist.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/proj_rouss.c
        ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/rtodms.c ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src/vector1.c )
TARGET_INCLUDE_DIRECTORIES( proj4 PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty/proj4/src
                                ${CMAKE_SOURCE_DIR}/src/adcirc/point 
                                ${CMAKE_SOURCE_DIR}/src/adcirc/qProj4 )
TARGET_LINK_LIBRARIES( proj4 point )
ADD_DEPENDENCIES( proj4 point )
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
ADD_LIBRARY( adcircmodules SHARED 
                ${CMAKE_SOURCE_DIR}/src/adcirc/config.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/mesh.cpp 
                ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/node.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/element.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/boundary.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/io/split.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/io/io.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/io/stringconversion.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/modelparameters/attribute.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/modelparameters/attributemetadata.cpp
                ${CMAKE_SOURCE_DIR}/src/adcirc/modelparameters/nodalattributes.cpp )

TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_SOURCE_DIR}/src/adcirc/point
                                ${CMAKE_SOURCE_DIR}/src/adcirc/qKdtree2
                                ${CMAKE_SOURCE_DIR}/src/adcirc/qProj4
                                ${CMAKE_SOURCE_DIR}/thirdparty/shapelib
                                ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_66_0 
                                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree )

ADD_DEPENDENCIES( adcircmodules point proj4 kdtree2 shapelib )

TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE GIT_VERSION="${GIT_VERSION}")
TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE BUILD_TIME="${BUILD_TIME}")

SET_TARGET_PROPERTIES( adcircmodules PROPERTIES VERSION ${ADCIRCMODULES_VERSION_STRING} SOVERSION ${ADCIRCMODULES_VERSION_MAJOR} )

IF(WIN32)
    INSTALL(TARGETS adcircmodules LIBRARY DESTINATION bin RUNTIME DESTINATION lib)
ELSEIF(CYGWIN)
    INSTALL(TARGETS adcircmodules LIBRARY DESTINATION lib RUNTIME DESTINATION lib)
ELSE(WIN32)
    INSTALL( TARGETS adcircmodules LIBRARY DESTINATION lib)
ENDIF(WIN32)

INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc.h DESTINATION include)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/adcirc_errors.h DESTINATION include/adcirc)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/config.h DESTINATION include/adcirc)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/qKdtree2/qkdtree2.h DESTINATION include/adcirc/qKdtree2)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/qProj4/qproj4.h DESTINATION include/adcirc/qProj4)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/point/point.h DESTINATION include/adcirc/point)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/node.h DESTINATION include/adcirc/geometry)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/element.h DESTINATION include/adcirc/geometry)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/boundary.h DESTINATION include/adcirc/geometry)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/geometry/mesh.h DESTINATION include/adcirc/geometry)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/modelparameters/attribute.h DESTINATION include/adcirc/modelparameters)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/modelparameters/attributemetadata.h DESTINATION include/adcirc/modelparameters)
INSTALL( FILES ${CMAKE_SOURCE_DIR}/src/adcirc/modelparameters/nodalattributes.h DESTINATION include/adcirc/modelparameters)

TARGET_LINK_LIBRARIES( adcircmodules proj4 point kdtree2 shapelib )
###########################################################################


###########################################################################
# Test case
###########################################################################
###########################################################################

###########################################################################
#  PyAdcirc Library
###########################################################################
IF(PythonLibs_FOUND AND SWIG_FOUND)
    
    FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/pyadcirc)
    
    CONFIGURE_FILE( ${CMAKE_SOURCE_DIR}/swig/__init__.py ${CMAKE_BINARY_DIR}/pyadcirc COPYONLY )

    IF(WIN32)

        #...Builds a dummy cmake project
        #   Which will add some windows specific include
        #   files not required by Unix/Cygwin [mingw64]
        #   Further testing required to see if this is 
        #   required by MSVC compilers
        FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeFiles/pywrapper.cmake "
            FILE(READ \${SRC} DATA)
            FILE(WRITE \${DST} \"#include <math.h>\")
            FILE(APPEND \${DST} \"\\n\\n\")
            FILE(APPEND \${DST} \"\${DATA}\")
        ")

        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR}/pyadcirc -c++ -python -I${CMAKE_SOURCE_DIR}/src 
                    -I${PYTHON_INCLUDE_PATH} 
                    -o ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1
                    ${CMAKE_SOURCE_DIR}/swig/adcirc.i
            COMMAND ${CMAKE_COMMAND} -DSRC=${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1 
                                     -DDST=${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
                                     -P ${CMAKE_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python..."
        )

    ELSE(WIN32)
        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR}/pyadcirc -c++ -python -I${CMAKE_SOURCE_DIR}/src 
                    -I${PYTHON_INCLUDE_PATH} 
                    -o ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
                    ${CMAKE_SOURCE_DIR}/swig/adcirc.i
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python..."
        )
    ENDIF(WIN32)

    ADD_LIBRARY( PyAdcirc SHARED ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx )
    TARGET_INCLUDE_DIRECTORIES( PyAdcirc PRIVATE ${PYTHON_INCLUDE_DIRECTORIES} ${CMAKE_SOURCE_DIR}/src )
    TARGET_LINK_LIBRARIES( PyAdcirc PRIVATE adcircmodules ${PYTHON_LIBRARY} )
    SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES PREFIX "_")
    SET_PROPERTY( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES pyadcirc/PyAdcirc.py pyadcirc/__init__.py CMakeFiles/python_adcirc_wrap.cxx )
    ADD_DEPENDENCIES( PyAdcirc adcircmodules )

    IF(WIN32)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyadcirc
                                                   RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyadcirc )
        INSTALL(TARGETS PyAdcirc LIBRARY DESTINATION bin RUNTIME DESTINATION pyadcirc)
    ELSEIF(CYGWIN)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyadcirc)
        INSTALL(TARGETS PyAdcirc LIBRARY DESTINATION lib RUNTIME DESTINATION pyadcirc)
    ELSE(WIN32)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/pyadcirc)
        INSTALL(TARGETS PyAdcirc LIBRARY DESTINATION pyadcirc)
    ENDIF(WIN32)
    
    INSTALL(FILES ${CMAKE_BINARY_DIR}/pyadcirc/PyAdcirc.py DESTINATION pyadcirc)
    INSTALL(FILES ${CMAKE_BINARY_DIR}/pyadcirc/__init__.py DESTINATION pyadcirc)
    
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
ENDIF(PythonLibs_FOUND AND SWIG_FOUND)
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
                        -I${PYTHON_INCLUDE_PATH} 
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
                        -I${PERL_INCLUDE_PATH} 
                        -o ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
                        ${CMAKE_SOURCE_DIR}/swig/adcirc.i
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                COMMENT "Generating SWIG interface to Perl..."
            )
        ENDIF(WIN32)
    
        ADD_LIBRARY( perladcirc SHARED ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx )
        TARGET_INCLUDE_DIRECTORIES( perladcirc PRIVATE ${PERL_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src )
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
    #...C++ Testing
    FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cxx_testcases)

    FOREACH(TESTFILE cxx_readmesh.cpp cxx_writemesh.cpp cxx_writeshapefile.cpp
                     cxx_projectmesh.cpp cxx_nodalSearchTree.cpp cxx_readfort13_wmesh.cpp 
                     cxx_readfort13_womesh.cpp cxx_fort13findatt.cpp )
        GET_FILENAME_COMPONENT( TESTNAME ${TESTFILE} NAME_WE )
        ADD_EXECUTABLE( ${TESTNAME} ${CMAKE_SOURCE_DIR}/testing/cxx_tests/${TESTFILE} )
        ADD_DEPENDENCIES( ${TESTNAME} adcircmodules )
        TARGET_INCLUDE_DIRECTORIES( ${TESTNAME} PRIVATE ${CMAKE_SOURCE_DIR}/src )
        TARGET_LINK_LIBRARIES( ${TESTNAME} adcircmodules )
        SET_TARGET_PROPERTIES( ${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cxx_testcases )

        ADD_TEST( NAME TEST_${TESTNAME} COMMAND ${CMAKE_BINARY_DIR}/cxx_testcases/${TESTNAME}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing )
        SET_TESTS_PROPERTIES( TEST_${TESTNAME} PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}")
    ENDFOREACH()
    
    #...Python Test
    IF(PYTHON_TESTING)
        ADD_TEST( NAME Python_Test COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/pyadcirc_testcase.py
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
        SET_PROPERTY( TEST Python_Test PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}")
        SET_PROPERTY( TEST Python_Test APPEND PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/pyadcirc")
    ENDIF(PYTHON_TESTING)
ENDIF(UNIX OR CYGWIN)
###########################################################################
