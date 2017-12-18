
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
#  LIBRARY VERSION
###########################################################################
set(ADCIRCMODULES_VERSION_MAJOR 0)
set(ADCIRCMODULES_VERSION_MINOR 1)
set(ADCIRCMODULES_VERSION_PATCH 0)
set(ADCIRCMODULES_VERSION_STRING ${ADCIRCMODULES_VERSION_MAJOR}.${ADCIRCMDULES_VERSION_MINOR}.${ADCIRCMODULES_VERSION_PATCH})
###########################################################################


###########################################################################
#  SWIG/PYTHON
###########################################################################
FIND_PACKAGE(SWIG)
IF(SWIG_FOUND)
    INCLUDE(${SWIG_USE_FILE})
ENDIF(SWIG_FOUND)

FIND_PACKAGE(PythonLibs)
IF(PythonLibs_FOUND)
    INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
ENDIF(PythonLibs_FOUND)
###########################################################################


###########################################################################
###########################################################################
#  LIBRARIES    
###########################################################################
###########################################################################


###########################################################################
#  POINT Library
###########################################################################
ADD_LIBRARY( point STATIC ${CMAKE_SOURCE_DIR}/interfaces/point/point.cpp )
TARGET_INCLUDE_DIRECTORIES( point PRIVATE ${CMAKE_SOURCE_DIR}/interfaces/point )
###########################################################################


###########################################################################
#  KDTREE2 Library
###########################################################################
ADD_LIBRARY( kdtree2 STATIC ${CMAKE_SOURCE_DIR}/interfaces/qKdtree2/qKdtree2.cpp 
                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree/kdtree2.cpp )
TARGET_INCLUDE_DIRECTORIES( kdtree2 PRIVATE ${CMAKE_SOURCE_DIR}/interfaces/qKdtree2
                                ${CMAKE_SOURCE_DIR}/interfaces/point
                                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree
                                ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_65_1 )
ADD_DEPENDENCIES( kdtree2 point)
TARGET_LINK_LIBRARIES( kdtree2 point )
###########################################################################


###########################################################################
#  PROJ4 Library
###########################################################################
ADD_LIBRARY( proj4 STATIC ${CMAKE_SOURCE_DIR}/interfaces/qProj4/qProj4.cpp 
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
                                ${CMAKE_SOURCE_DIR}/interfaces/point 
                                ${CMAKE_SOURCE_DIR}/interfaces/qProj4 )
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
ADD_LIBRARY( adcircmodules SHARED ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/adcircmesh.cpp 
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/adcircnode.cpp
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/adcircelement.cpp
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/adcircboundary.cpp
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/split.cpp
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/stringconversion.cpp
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/adcircnodalattribute.cpp
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/adcircnodalattributemetadata.cpp
                ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib/adcircnodalparameters.cpp )
TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib
                                ${CMAKE_SOURCE_DIR}/interfaces/point
                                ${CMAKE_SOURCE_DIR}/interfaces/qKdtree2
                                ${CMAKE_SOURCE_DIR}/interfaces/qProj4
                                ${CMAKE_SOURCE_DIR}/thirdparty/shapelib
                                ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_65_1 
                                ${CMAKE_SOURCE_DIR}/thirdparty/kdtree )
ADD_DEPENDENCIES( adcircmodules point proj4 kdtree2 shapelib )
TARGET_LINK_LIBRARIES( adcircmodules proj4 point kdtree2 shapelib )
###########################################################################


###########################################################################
#  PyAdcirc Library
###########################################################################
IF(PythonLibs_FOUND AND SWIG_FOUND)
    IF(WIN32)

        #...Builds a dummy cmake project
        FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeFiles/pywrapper.cmake "
            FILE(READ \${SRC} DATA)
            FILE(WRITE \${DST} \"#include <math.h>\")
            FILE(APPEND \${DST} \"\\n\\n\")
            FILE(APPEND \${DST} \"\${DATA}\")
        ")

        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/pyadcirc_wrap.cxx
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python -I${CMAKE_SOURCE_DIR}/ADCIRCModules_lib 
                    -I${CMAKE_SOURCE_DIR}/interfaces/point 
                    -I${CMAKE_SOURCE_DIR}/interfaces/qKdtree2 
                    -I${CMAKE_SOURCE_DIR}/interfaces/qProj4 
                    -I${PYTHON_INCLUDE_PATH} 
                    -o ${CMAKE_BINARY_DIR}/pyadcirc_wrap.cxx.1
                    ${CMAKE_SOURCE_DIR}/pyadcirc/pyadcirc.i
            COMMAND ${CMAKE_COMMAND} -DSRC=${CMAKE_BINARY_DIR}/pyadcirc_wrap.cxx.1 
                                     -DDST=${CMAKE_BINARY_DIR}/pyadcirc_wrap.cxx
                                     -P ${CMAKE_BINARY_DIR}/CMakeFiles/pywrapper.cmake
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python..."
        )

    ELSE(WIN32)
        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/pyadcirc_wrap.cxx
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python -I${CMAKE_SOURCE_DIR}/ADCIRCModules_lib 
                    -I${CMAKE_SOURCE_DIR}/interfaces/point 
                    -I${CMAKE_SOURCE_DIR}/interfaces/qKdtree2 
                    -I${CMAKE_SOURCE_DIR}/interfaces/qProj4 
                    -I${PYTHON_INCLUDE_PATH} 
                    -o ${CMAKE_BINARY_DIR}/pyadcirc_wrap.cxx
                    ${CMAKE_SOURCE_DIR}/pyadcirc/pyadcirc.i
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python..."
        )
    ENDIF(WIN32)

    ADD_LIBRARY( pyadcirc SHARED ${CMAKE_BINARY_DIR}/pyadcirc_wrap.cxx )
    TARGET_INCLUDE_DIRECTORIES( pyadcirc PRIVATE ${PYTHON_INCLUDE_DIRECTORIES} 
            ${CMAKE_SOURCE_DIR}/ADCIRCModules_lib 
            ${CMAKE_SOURCE_DIR}/interfaces/point 
            ${CMAKE_SOURCE_DIR}/interfaces/qKdtree2 
            ${CMAKE_SOURCE_DIR}/interfaces/qProj4 )
    TARGET_LINK_LIBRARIES( pyadcirc PRIVATE adcircmodules ${PYTHON_LIBRARY} )
    SET_TARGET_PROPERTIES( pyadcirc PROPERTIES PREFIX "_")
    SET_DIRECTORY_PROPERTIES( PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES  "pyadcirc.py;pyadcirc_wrap.cxx*" )
    ADD_DEPENDENCIES( pyadcirc adcircmodules )
    
    IF(WIN32)
        SET_TARGET_PROPERTIES( pyadcirc PROPERTIES SUFFIX ".pyd")
        IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
            # 64 bits
            TARGET_COMPILE_DEFINITIONS(pyadcirc PRIVATE "MS_WIN64")
        ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
            # 32 bits
            TARGET_COMPILE_DEFINITIONS(pyadcirc PRIVATE "MS_WIN32")
        ENDIF()
    ENDIF(WIN32)
ENDIF(PythonLibs_FOUND AND SWIG_FOUND)
###########################################################################
