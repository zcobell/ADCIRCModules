
# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
FindSQLITE3
-----------

.. versionadded:: 3.14

Find the SQLite libraries, v3

IMPORTED targets
^^^^^^^^^^^^^^^^

This module defines the following :prop_tgt:`IMPORTED` target:

``SQLite::SQLITE3``

Result variables
^^^^^^^^^^^^^^^^

This module will set the following variables if found:

``SQLITE3_INCLUDE_DIRS``
  where to find sqlite3.h, etc.
``SQLITE3_LIBRARIES``
  the libraries to link against to use SQLITE3.
``SQLITE3_VERSION``
  version of the SQLITE3 library found
``SQLITE3_FOUND``
  TRUE if found

#]=======================================================================]

# Look for the necessary header
find_path(SQLITE3_INCLUDE_DIR sqlite3.h 
	HINTS 
	"${SQLITE3_DIR}/include"  
	"$ENV{INCLUDE}" 
	"$ENV{LIB_DIR}/include" )
mark_as_advanced(SQLITE3_INCLUDE_DIR)

# Look for the necessary library
find_library(SQLITE3_LIBRARY NAMES sqlite3 sqlite PATHS
	HINTS
	"${SQLITE3_DIR}/lib"
	"$ENV{LIB}" 
	"$ENV{LIB_DIR}/lib") 
mark_as_advanced(SQLITE3_LIBRARY)

# Extract version information from the header file
if(SQLITE3_INCLUDE_DIR)
    file(STRINGS ${SQLITE3_INCLUDE_DIR}/sqlite3.h _ver_line
         REGEX "^#define SQLITE_VERSION  *\"[0-9]+\\.[0-9]+\\.[0-9]+\""
         LIMIT_COUNT 1)
    string(REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+"
           SQLITE3_VERSION "${_ver_line}")
    unset(_ver_line)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
find_package_handle_standard_args(SQLITE3
    REQUIRED_VARS SQLITE3_INCLUDE_DIR SQLITE3_LIBRARY
    VERSION_VAR SQLITE3_VERSION)

# Create the imported target
if(SQLITE3_FOUND)
    set(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
    set(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})
    if(NOT TARGET SQLite::SQLITE3)
        add_library(SQLite::SQLITE3 UNKNOWN IMPORTED)
        set_target_properties(SQLite::SQLITE3 PROPERTIES
            IMPORTED_LOCATION             "${SQLITE3_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SQLITE3_INCLUDE_DIR}")
    endif()
endif()
