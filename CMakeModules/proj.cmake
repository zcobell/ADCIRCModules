# ##############################################################################
# PROJ Library
# ##############################################################################
find_package(SQLite3 REQUIRED)
find_package(PROJ)
IF(PROJ_FOUND)
	IF(${PROJ_VERSION} VERSION_LESS 6.0)
		message(WARNING "System PROJ version not compatible. Attempting to use internal proj.")
		set(PROJ_FOUND FALSE)
		set(PROJ_INCLUDE_DIR "")
		set(PROJ_LIBRARY "")
	ENDIF()
ENDIF()
if(NOT PROJ_FOUND)
	set(BUILD_TESTING
	    OFF
	    CACHE BOOL "Build PROJ tests")
	set(ENABLE_CURL
	    OFF
	    CACHE BOOL "Enable CURL for use with PROJ")
	set(BUILD_PROJSYNC
	    OFF
	    CACHE BOOL "Build PROJSYNC")
	set(BUILD_SHARED_LIBS
	    OFF
	    CACHE BOOL "Build shared PROJ libs")
	set(BUILD_CCT
	    OFF
	    CACHE BOOL "Build PROJ CCT")
	set(BUILD_CS2CS
	    OFF
	    CACHE BOOL "Build PROJ CS2CS")
	set(BUILD_GEOD
	    OFF
	    CACHE BOOL "Build PROJ GEOD")
	set(BUILD_PROJINFO
	    OFF
	    CACHE BOOL "Build PROJ PROJINFO")
	set(BUILD_GIE
	    OFF
	    CACHE BOOL "Build PROJ GIE")
	set(BUILD_PROJ
	    OFF
	    CACHE BOOL "Build PROJ binary")
	mark_as_advanced(
	  BUILD_TESTING
	  ENABLE_CURL
	  BUILD_PROJSYNC
	  BUILD_SHARED_LIBS
	  BUILD_CCT
	  BUILD_CS2CS
	  BUILD_GEOD
	  BUILD_PROJINFO
	  BUILD_GIE
	  BUILD_PROJ
      CMAKE_CXX_STANDARD  
      DATADIR              
      DL_LIB              
      DOCDIR              
      ENABLE_IPO          
      INCLUDEDIR          
      LIBDIR              
      LIBRT               
      M_LIB               
      PROJ_CXX_WARN_FLAGS 
      PROJ_C_WARN_FLAGS 
      PROJ_VERSION 
      CMAKECONFIGDIR )
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/proj-7.2.0)
    set(PROJ_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/proj-7.2.0/src)
	set(PROJ_LIBRARY adcmod_proj )
    set(ADCIRCMODULES_PROJ_VERSION 7.2.0 CACHE STRING "PROJ Version found")
else()
    set(ADCIRCMODULES_PROJ_VERSION ${PROJ_VERSION} CACHE STRING "PROJ Version found")
endif()
# ##############################################################################
