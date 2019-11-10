
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
INCLUDE (GNUInstallDirs)
INCLUDE (CMakePackageConfigHelpers)

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
        setup_target_for_coverage(adcircmodules_coverage tests coverage)
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -g -O0 -fprofile-arcs -ftest-coverage")
    ENDIF(CMAKE_COMPILER_IS_GNUCXX)
ENDIF(COVERAGE)
###########################################################################


###########################################################################
# C++ 11 Check
###########################################################################
INCLUDE(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
IF(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ELSE(COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
ENDIF(COMPILER_SUPPORTS_CXX11)
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
#  LIBRARY VERSION
###########################################################################
set(ADCIRCMODULES_VERSION_MAJOR 0)
set(ADCIRCMODULES_VERSION_MINOR 3)
set(ADCIRCMODULES_VERSION_PATCH 0)
set(ADCIRCMODULES_VERSION_STRING ${ADCIRCMODULES_VERSION_MAJOR}.${ADCIRCMODULES_VERSION_MINOR}.${ADCIRCMODULES_VERSION_PATCH})
###########################################################################

###########################################################################
# CODE VERSION (GIT)
###########################################################################
EXECUTE_PROCESS( COMMAND git describe --always --tags
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_VERSION
                    RESULT_VARIABLE GIT_RETURN_VALUE
                    OUTPUT_STRIP_TRAILING_WHITESPACE )
IF( NOT "${GIT_RETURN_VALUE}" STREQUAL "0" )
    SET(GIT_VERSION "${ADCIRCMODULES_VERSION_STRING}.cv")
ENDIF()
MESSAGE(STATUS "ADCIRCModules Version: ${GIT_VERSION}")
###########################################################################


###########################################################################
# ABSEIL SWISS TABLES
###########################################################################
IF(NOT CYGWIN)
    OPTION(USE_GOOGLE_FLAT_MAP "Use Google's hash table to increase speed" ON)
    IF(USE_GOOGLE_FLAT_MAP)
        if(MSVC)
          # /wd4005  macro-redefinition
          # /wd4068  unknown pragma
          # /wd4244  conversion from 'type1' to 'type2'
          # /wd4267  conversion from 'size_t' to 'type2'
          # /wd4800  force value to bool 'true' or 'false' (performance warning)
          add_compile_options(/wd4005 /wd4068 /wd4244 /wd4267 /wd4800)
          add_definitions(/DNOMINMAX /DWIN32_LEAN_AND_MEAN=1 /D_CRT_SECURE_NO_WARNINGS)
        endif(MSVC)
        add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/abseil EXCLUDE_FROM_ALL)
        MESSAGE(STATUS "Using Google's Flat Map (from ABSL) instead of unordered_map for performance")
    ENDIF(USE_GOOGLE_FLAT_MAP)    
ELSE(NOT CYGWIN)
    SET(USE_GOOGLE_FLAT_MAP FALSE)
ENDIF(NOT CYGWIN)
###########################################################################

###########################################################################
# Boost
###########################################################################
FIND_PACKAGE( Boost 1.60.0 )
IF( NOT Boost_FOUND OR PACKAGE_VERSION_UNSUITABLE )
    MESSAGE( STATUS "Using included Boost Version: 1.66.0")
    SET( Boost_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_66_0 )
ENDIF()
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
#  Note: This will attempt to default to Python3
#  Give the flag -DPYTHON2=ON to force Python2 usage
###########################################################################
IF(${PYTHON2})
    MESSAGE(STATUS "CMake will attempt to use the Python2 interface only")
    FIND_PACKAGE(PythonInterp 2.7 REQUIRED )
    FIND_PACKAGE(PythonLibs 2.7 REQUIRED )
    SET(PYTHONTYPE 2)
ELSE()
    FIND_PACKAGE(PythonInterp 3.0)
    FIND_PACKAGE(PythonLibs 3.0)
    IF(NOT PythonLibs_FOUND AND NOT PythonInterp_FOUND)
        FIND_PACKAGE(PythonInterp 2.7)
        FIND_PACKAGE(PythonLibs 2.7)
        SET(PYTHONTYPE 2)
    ELSE()
        SET(PYTHONTYPE 3)
    ENDIF()
ENDIF()

FIND_PACKAGE(SWIG 3.0)
IF(SWIG_FOUND AND PythonLibs_FOUND AND PythonInterp_FOUND)
    INCLUDE(${SWIG_USE_FILE})
    IF(PythonLibs_FOUND)
        OPTION(PYTHON_TESTING "Test the Python Libraries with CTest" OFF)
        EXECUTE_PROCESS( COMMAND ${PYTHON_EXECUTABLE} -m site --user-site 
                         OUTPUT_VARIABLE temp_PYTHON_SITE_DIRECTORY OUTPUT_STRIP_TRAILING_WHITESPACE)
        SET( PYTHON_INSTALL_DIRECTORY ${temp_PYTHON_SITE_DIRECTORY} CACHE STRING "Location where Python libraries will be installed" )
        MARK_AS_ADVANCED( CLEAR PYTHON_EXECUTABLE )
    ELSE(PythonLibs_FOUND)
        SET( PYTHON_INSTALL_DIRECTORY "PYTHON_INSTALL_DIRECTORY-NOTFOUND" CACHE STRING "Location where Python libraries will be installed" )
    ENDIF(PythonLibs_FOUND)
    
    #FIND_PACKAGE(PerlLibs)
    #IF(PerlLibs_FOUND)
    #    OPTION(TEST_PERL "Test the perl build for SWIG. Note: Does not work yet" OFF) 
    #ENDIF(PerlLibs_FOUND)
ELSE(SWIG_FOUND AND PythonLibs_FOUND AND PythonInterp_FOUND)
    MESSAGE( WARNING "SWIG/Python libraries could not be found. PyAdcirc disabled.")
ENDIF(SWIG_FOUND AND PythonLibs_FOUND AND PythonInterp_FOUND)
###########################################################################
    
    
###########################################################################
#  NETCDF
###########################################################################
IF(WIN32)
    SET(NETCDF_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/netcdf/include" ) 
ELSE(WIN32)
	IF(NOT "${NETCDFHOME}" STREQUAL "")
        SET(NETCDF_DIR "${NETCDFHOME}" CACHE STRING "Location of NETCDF Library")
	ELSEIF(NOT $ENV{NETCDFHOME} STREQUAL "")
        SET(NETCDF_DIR $ENV{NETCDFHOME} CACHE STRING "Locatin of NETCDF Library")
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
ENDIF(WIN32)
###########################################################################

###########################################################################
#  HDF5
###########################################################################
IF(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
ENDIF(POLICY CMP0074)
IF(NOT "${HDF5HOME}" STREQUAL "")
    SET(HDF5_ROOT "${HDF5HOME}" CACHE STRING "Location of HDF5 Library")
ELSEIF(NOT $ENV{HDF5HOME} STREQUAL "")
    SET(HDF5_ROOT $ENV{HDF5HOME} CACHE STRING "Location of HDF5 Library")
ENDIF(NOT "${HDF5HOME}" STREQUAL "")
Find_Package(HDF5)
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
# OpenSSL
###########################################################################
IF(NOT "${OPENSSLHOME}" STREQUAL "")
    SET(OPENSSL_ROOT_DIR "${OPENSSLHOME}")
ELSEIF(NOT $ENV{OPENSSLHOME} STREQUAL "")
    SET(OPENSSL_ROOT_DIR $ENV{OPENSSLHOME})
ENDIF(NOT "${OPENSSLHOME}" STREQUAL "")
FIND_PACKAGE(OpenSSL)
IF(OpenSSL_FOUND)
    MESSAGE(STATUS "OpenSSL libraries found. Hashing is enabled.")
ELSE()
    MESSAGE(STATUS "OpenSSL libraries not found. Hashing disabled.")
ENDIF()
###########################################################################

###########################################################################
# GDAL
###########################################################################
OPTION(ENABLE_GDAL "Enabele GDAL" ON )
IF(ENABLE_GDAL)
    Find_PACKAGE(GDAL)
    GET_FILENAME_COMPONENT(GDAL_LIBPATH ${GDAL_LIBRARY} DIRECTORY)
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
###########################################################################
#  LIBRARIES    
###########################################################################
###########################################################################

###########################################################################
#  EzProj Library
###########################################################################
add_subdirectory( ${CMAKE_SOURCE_DIR}/thirdparty/ezproj EXCLUDE_FROM_ALL )
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
                ${CMAKE_SOURCE_DIR}/src/logging.cpp
                ${CMAKE_SOURCE_DIR}/src/filetypes.cpp
		${CMAKE_SOURCE_DIR}/src/formatting.cpp
                ${CMAKE_SOURCE_DIR}/src/mesh.cpp 
                ${CMAKE_SOURCE_DIR}/src/node.cpp
                ${CMAKE_SOURCE_DIR}/src/element.cpp
                ${CMAKE_SOURCE_DIR}/src/hash.cpp
                ${CMAKE_SOURCE_DIR}/src/hmdf.cpp
                ${CMAKE_SOURCE_DIR}/src/hmdfstation.cpp
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
                ${CMAKE_SOURCE_DIR}/src/meshchecker.cpp 
                ${CMAKE_SOURCE_DIR}/src/multithreading.cpp 
                ${CMAKE_SOURCE_DIR}/src/constants.cpp
                ${CMAKE_SOURCE_DIR}/src/mesh_private.cpp 
                ${CMAKE_SOURCE_DIR}/src/kdtree.cpp
                ${CMAKE_SOURCE_DIR}/src/kdtree_private.cpp
                ${CMAKE_SOURCE_DIR}/src/harmonicsoutput_private.cpp 
                ${CMAKE_SOURCE_DIR}/src/harmonicsrecord_private.cpp 
                ${CMAKE_SOURCE_DIR}/src/nodalattributes_private.cpp )

IF( GDAL_FOUND )
    SET( ADCIRCMODULES_SOURCES ${ADCIRCMODULES_SOURCES} ${CMAKE_SOURCE_DIR}/src/griddata.cpp 
                                                        ${CMAKE_SOURCE_DIR}/src/griddata_private.cpp
                                                        ${CMAKE_SOURCE_DIR}/src/pixel.cpp
                                                        ${CMAKE_SOURCE_DIR}/src/rasterdata.cpp )
ENDIF( GDAL_FOUND )

ADD_LIBRARY( adcircmodules SHARED ${ADCIRCMODULES_SOURCES} )

SET(HEADER_LIST ${CMAKE_SOURCE_DIR}/src/adcircmodules.h
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
                ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src/ezproj.h )

IF(GDAL_FOUND)
   TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE "USE_GDAL" )
   TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE ${GDAL_INCLUDE_DIR})
   LINK_DIRECTORIES( ${GDAL_LIBPATH} )
   TARGET_LINK_LIBRARIES( adcircmodules ${GDAL_LIBRARY})
   SET( HEADER_LIST ${HEADER_LIST} 
                    ${CMAKE_SOURCE_DIR}/src/rasterdata.h 
                    ${CMAKE_SOURCE_DIR}/src/griddata.h ) 
ENDIF(GDAL_FOUND)
TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_SOURCE_DIR}/src/pimpl
                                ${CMAKE_SOURCE_DIR}/src
                                ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src
                                ${CMAKE_SOURCE_DIR}/thirdparty/shapelib
                                ${CMAKE_SOURCE_DIR}/thirdparty/abseil
                                ${CMAKE_SOURCE_DIR}/thirdparty/nanoflann/include
                                ${CMAKE_SOURCE_DIR}/libraries/constants
                                ${Boost_INCLUDE_DIRS} 
                                ${NETCDF_INCLUDE_DIR} ) 

ADD_DEPENDENCIES( adcircmodules ezproj shapelib )

TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE GIT_VERSION="${GIT_VERSION}")
TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE ADCIRCMODULES_LIBRARY )

SET_TARGET_PROPERTIES( adcircmodules PROPERTIES VERSION ${ADCIRCMODULES_VERSION_STRING} SOVERSION ${ADCIRCMODULES_VERSION_MAJOR} )

 

SET_TARGET_PROPERTIES( adcircmodules PROPERTIES PUBLIC_HEADER "${HEADER_LIST}" ) 

IF(${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local" OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local/" OR
   ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/" OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/")
    SET(HEADER_DEST "${CMAKE_INSTALL_INCLUDEDIR}/adcircmodules")
ELSE()
    SET(HEADER_DEST ${CMAKE_INSTALL_INCLUDEDIR})
ENDIF()

WRITE_BASIC_PACKAGE_VERSION_FILE( adcircmodulesConfigVersion.cmake VERSION ${ADCIRCMODULES_VERSION_STRING} COMPATIBILITY SameMajorVersion )
INSTALL( TARGETS adcircmodules RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ADCIRCMODULES_RUNTIME
                               LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ADCIRCMODULES_RUNTIME
                               ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ADCIRCMODULES_DEVELOPMENT
                               PUBLIC_HEADER DESTINATION ${HEADER_DEST}    COMPONENT ADCIRCMODULES_DEVELOPMENT )
INSTALL( FILES ${CMAKE_CURRENT_BINARY_DIR}/adcircmodulesConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake )

SET_TARGET_PROPERTIES(adcircmodules PROPERTIES CMAKE_CXX_VISIBILITY_PRESET hidden)
SET_TARGET_PROPERTIES(adcircmodules PROPERTIES CMAKE_CXX_INLINES_HIDDEN YES)

TARGET_LINK_LIBRARIES( adcircmodules ezproj shapelib )

IF(WIN32)
    LINK_DIRECTORIES( ${CMAKE_SOURCE_DIR}/thirdparty/netcdf/libs_vc64 )
    TARGET_LINK_LIBRARIES( adcircmodules netcdf hdf5 hdf5_hl )
ELSE(WIN32)
    TARGET_LINK_LIBRARIES( adcircmodules ${NETCDF_LIBRARIES} )
ENDIF(WIN32)

IF(OpenSSL_FOUND)
    TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE ADCMOD_HAVE_OPENSSL )
    TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE ${OPENSSL_INCLUDE_DIR}) 
    TARGET_LINK_LIBRARIES( adcircmodules ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} )
ENDIF( OpenSSL_FOUND )

IF(APPLE)
    SET(CMAKE_MACOSX_RPATH 0)
    SET_TARGET_PROPERTIES(adcircmodules PROPERTIES INSTALL_NAME_DIR "adcircmodules")
    SET_TARGET_PROPERTIES(adcircmodules PROPERTIES MACOSX_RPATH "adcircmodules")
ENDIF(APPLE)

IF(USE_GOOGLE_FLAT_MAP)
    TARGET_LINK_LIBRARIES(adcircmodules absl::flat_hash_map)
    ADD_DEPENDENCIES(adcircmodules absl::flat_hash_map)
    TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE USE_GOOGLE_FLAT_MAP )
ENDIF(USE_GOOGLE_FLAT_MAP)

IF(OPENMP_FOUND)
    TARGET_COMPILE_OPTIONS( adcircmodules PRIVATE ${OpenMP_CXX_FLAGS})
    TARGET_LINK_LIBRARIES(  adcircmodules ${OpenMP_CXX_LIB_NAMES} ${OpenMP_CXX_FLAGS} )
ENDIF(OPENMP_FOUND)

IF(HDF5_FOUND)
    TARGET_COMPILE_DEFINITIONS( adcircmodules PRIVATE HAVE_HDF5 ${HDF5_DEFINITIONS} )
    TARGET_INCLUDE_DIRECTORIES( adcircmodules PRIVATE ${HDF5_INCLUDE_DIRS} )
    TARGET_LINK_LIBRARIES( adcircmodules ${HDF5_LIBRARIES} )
ENDIF(HDF5_FOUND)

###########################################################################


###########################################################################
# Test case
###########################################################################
###########################################################################

###########################################################################
#  PyAdcirc Library
###########################################################################
IF(SWIG_FOUND AND PythonLibs_FOUND AND PythonInterp_FOUND)

    IF(GDAL_FOUND)
        SET(SWIG_GDAL_FLAG "-D_USE_GDAL")
    ENDIF(GDAL_FOUND)

    IF(PYTHONTYPE EQUAL 2)
        SET(PYTHONFLAG "")
    ELSE()
        SET(PYTHONFLAG "-py3")
    ENDIF()
    
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
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python ${PYTHONFLAG}
                    -I${CMAKE_SOURCE_DIR}/src 
                    -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src i
                    -I${PYTHON_INCLUDE_PATH} ${SWIG_GDAL_FLAG}
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
            COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -python ${PYTHONFLAG}
                    -I${CMAKE_SOURCE_DIR}/src 
                    -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src 
                    -I${PYTHON_INCLUDE_PATH} ${SWIG_GDAL_FLAG} 
                    -o ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
                    ${CMAKE_SOURCE_DIR}/swig/adcirc.i
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating SWIG interface to Python..."
        )
    ENDIF(WIN32)

    ADD_LIBRARY( PyAdcirc SHARED ${CMAKE_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx )
    TARGET_INCLUDE_DIRECTORIES( PyAdcirc PRIVATE ${PYTHON_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src 
                                    ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src ) 
    TARGET_LINK_LIBRARIES( PyAdcirc adcircmodules ${PYTHON_LIBRARY} )
    IF(OPENSSL_FOUND)
        TARGET_INCLUDE_DIRECTORIES( PyAdcirc  PRIVATE ${OPENSSL_INCLUDE_DIR}) 
        TARGET_LINK_LIBRARIES( PyAdcirc ${OPENSSL_CRYPTO_LIBRARY} ${OPENSSL_SSL_LIBRARY} )
    ENDIF(OPENSSL_FOUND)
    SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES PREFIX "_")
    SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES INSTALL_NAME_DIR "PyAdcirc")
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
    ELSEIF(APPLE)
        SET_TARGET_PROPERTIES( PyAdcirc PROPERTIES SUFFIX ".so")
    ENDIF(WIN32)
ENDIF(SWIG_FOUND AND PythonLibs_FOUND AND PythonInterp_FOUND)
###########################################################################

###########################################################################
#  PerlAdcirc Library
###########################################################################
#IF(TEST_PERL)
#    IF(Perl_FOUND AND SWIG_FOUND)
#        IF(WIN32)
#    
#            #...Builds a dummy cmake project
#            #   Which will add some windows specific include
#            #   files not required by Unix/Cygwin [mingw64]
#            #   Further testing required to see if this is 
#            #   required by MSVC compilers
#            FILE(WRITE ${CMAKE_BINARY_DIR}/CMakeFiles/perlwrapper.cmake "
#                FILE(READ \${SRC} DATA)
#                FILE(WRITE \${DST} \"#include <math.h>\")
#                FILE(APPEND \${DST} \"\\n\\n\")
#                FILE(APPEND \${DST} \"\${DATA}\")
#            ")
#    
#            ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
#                COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -perl -I${CMAKE_SOURCE_DIR}/src
#                        -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src -I${PERL_INCLUDE_PATH} 
#                        -o ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx.1
#                        ${CMAKE_SOURCE_DIR}/swig/adcirc.i
#                COMMAND ${CMAKE_COMMAND} -DSRC=${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx.1 
#                                         -DDST=${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
#                                         -P ${CMAKE_BINARY_DIR}/CMakeFiles/perl_wrapper.cmake
#                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#                COMMENT "Generating SWIG interface to Perl..."
#            )
#    
#        ELSE(WIN32)
#            ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
#                COMMAND ${SWIG_EXECUTABLE} -outdir ${CMAKE_BINARY_DIR} -c++ -perl -I${CMAKE_SOURCE_DIR}/src 
#                        -I${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src -I${PERL_INCLUDE_PATH} 
#                        -o ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx
#                        ${CMAKE_SOURCE_DIR}/swig/adcirc.i
#                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#                COMMENT "Generating SWIG interface to Perl..."
#            )
#        ENDIF(WIN32)
#    
#        ADD_LIBRARY( perladcirc SHARED ${CMAKE_BINARY_DIR}/CMakeFiles/perl_adcirc_wrap.cxx )
#        TARGET_INCLUDE_DIRECTORIES( perladcirc PRIVATE ${PERL_INCLUDE_PATH} ${CMAKE_SOURCE_DIR}/src 
#                                        ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src 
#                                        ${CMAKE_SOURCE_DIR}/libraries/rasterdata )
#        TARGET_LINK_LIBRARIES( perladcirc PRIVATE adcircmodules ${PERL_LIBRARY} )
#        SET_TARGET_PROPERTIES( perladcirc PROPERTIES PREFIX "")
#        SET_PROPERTY( DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES perladcirc.pm CMakeFiles/perl_adcirc_wrap.cxx )
#        ADD_DEPENDENCIES( perladcirc adcircmodules )
#    
#        TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE HAS_BOOL)
#        
#        IF(WIN32)
#            IF(CMAKE_SIZEOF_VOID_P EQUAL 8)
#                # 64 bits
#                TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE "MS_WIN64")
#            ELSEIF(CMAKE_SIZEOF_VOID_P EQUAL 4)
#                # 32 bits
#                TARGET_COMPILE_DEFINITIONS(perladcirc PRIVATE "MS_WIN32")
#            ENDIF()
#        ENDIF(WIN32)
#    ENDIF(Perl_FOUND AND SWIG_FOUND)
#ENDIF(TEST_PERL)
###########################################################################


###########################################################################
# Test Cases
###########################################################################
IF(UNIX OR CYGWIN)
    IF(BUILD_TESTS)
        #...C++ Testing
        FILE(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cxx_testcases)

        SET(TEST_LIST cxx_readmesh.cpp cxx_readnetcdfmesh.cpp cxx_writemesh.cpp cxx_writeshapefile.cpp
                      cxx_projectmesh.cpp cxx_nodalSearchTree.cpp cxx_elementalSearchTree.cpp 
                      cxx_readfort13_wmesh.cpp cxx_readfort13_womesh.cpp cxx_fort13findatt.cpp 
                      cxx_readasciifull.cpp cxx_readasciisparse.cpp cxx_readmaxele.cpp cxx_readnetcdfmaxele.cpp
                      cxx_readasciivector.cpp cxx_readnetcdf.cpp cxx_readnetcdfvector.cpp
                      cxx_readHarmonicsElevation.cpp cxx_readHarmonicsVelocity.cpp 
                      cxx_readnetcdfHarmonicsElevation.cpp  cxx_readnetcdfHarmonicsVelocity.cpp
                      cxx_checkmesh.cpp cxx_read2dm.cpp cxx_kdtree.cpp cxx_hash.cpp cxx_hashmesh.cpp 
                      cxx_writeasciifull.cpp cxx_writeasciisparse.cpp cxx_writeasciifullvector.cpp 
                      cxx_writeasciisparsevector.cpp cxx_writenetcdf.cpp cxx_writenetcdfvector.cpp 
                      cxx_writehdf5.cpp cxx_makemesh.cpp )

        IF(ENABLE_GDAL)
            SET(TEST_LIST ${TEST_LIST} cxx_interpolateRaster.cpp cxx_interpolateManning.cpp cxx_interpolateDwind.cpp ) 
        ENDIF(ENABLE_GDAL)

        FOREACH(TESTFILE ${TEST_LIST} ) 
            GET_FILENAME_COMPONENT( TESTNAME ${TESTFILE} NAME_WE )
            ADD_EXECUTABLE( ${TESTNAME} ${CMAKE_SOURCE_DIR}/testing/cxx_tests/${TESTFILE} )
            ADD_DEPENDENCIES( ${TESTNAME} adcircmodules )
            TARGET_INCLUDE_DIRECTORIES( ${TESTNAME} PRIVATE ${CMAKE_SOURCE_DIR}/src 
                                                            ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src ) 
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
            IF(PYTHONTYPE EQUAL 2)
               SET(PYTHON_TESTNAME TEST_python2_PyAdcirc)
               ADD_TEST( NAME ${PYTHON_TESTNAME} COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/python_tests/pyadcirc_python2_testcase.py
                         WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
            ELSE()
               SET(PYTHON_TESTNAME TEST_python3_PyAdcirc)
               ADD_TEST( NAME ${PYTHON_TESTNAME} COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/testing/python_tests/pyadcirc_python3_testcase.py
                         WORKING_DIRECTORY ${CMAKE_BINARY_DIR} )
            ENDIF()
            SET_PROPERTY( TEST ${PYTHON_TESTNAME} PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${CMAKE_BINARY_DIR}")
            SET_PROPERTY( TEST ${PYTHON_TESTNAME} APPEND PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}")
        ENDIF(PYTHON_TESTING)
    ENDIF(BUILD_TESTS)
ENDIF(UNIX OR CYGWIN)
###########################################################################

###########################################################################
# Utilities
###########################################################################
ADD_EXECUTABLE( interpolateAdcircStations ${CMAKE_SOURCE_DIR}/utilities/interpolateAdcircStations/main.cpp 
                   ${CMAKE_SOURCE_DIR}/utilities/interpolateAdcircStations/interpolate.cpp )
TARGET_INCLUDE_DIRECTORIES( interpolateAdcircStations PRIVATE 
                            ${CMAKE_SOURCE_DIR}/utilities/interpolateAdcircStations 
                            ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src
                            ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
                            ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src ) 
ADD_DEPENDENCIES( interpolateAdcircStations adcircmodules ezproj )
TARGET_LINK_LIBRARIES( interpolateAdcircStations PRIVATE adcircmodules ezproj ${Boost_LIBRARIES} )
INSTALL( TARGETS interpolateAdcircStations RUNTIME DESTINATION bin )

ADD_EXECUTABLE( generateAdcircSimulationDates ${CMAKE_SOURCE_DIR}/utilities/generateAdcircSimulationDates/main.cpp )
TARGET_INCLUDE_DIRECTORIES( generateAdcircSimulationDates PRIVATE 
                            ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src
                            ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
                            ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src ) 
ADD_DEPENDENCIES( generateAdcircSimulationDates adcircmodules ezproj )
TARGET_LINK_LIBRARIES( generateAdcircSimulationDates PRIVATE adcircmodules ezproj ${Boost_LIBRARIES} )
INSTALL( TARGETS generateAdcircSimulationDates RUNTIME DESTINATION bin )


ADD_EXECUTABLE( resultsScope ${CMAKE_SOURCE_DIR}/utilities/resultsScope/main.cpp )
TARGET_INCLUDE_DIRECTORIES( resultsScope PRIVATE 
                            ${Boost_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src
                            ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
                            ${CMAKE_SOURCE_DIR}/thirdparty/ezproj/src ) 
ADD_DEPENDENCIES( resultsScope adcircmodules ezproj )
TARGET_LINK_LIBRARIES( resultsScope PRIVATE adcircmodules ezproj ${Boost_LIBRARIES} )
INSTALL( TARGETS resultsScope RUNTIME DESTINATION bin )
###########################################################################
