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
    WARNING "SWIG/Python libraries could not be found. Python ADCIRCModules disabled.")
endif(
  SWIG_FOUND
  AND PythonLibs_FOUND
  AND PythonInterp_FOUND)
# ##############################################################################
