# ##############################################################################
# SWIG/PYTHON/PERL Note: This will attempt to default to Python3 Give the flag
# -DPYTHON2=ON to force Python2 usage
# ##############################################################################
set(Python3_USE_STATIC_LIBS TRUE)
set(Python3_FIND_STRATEGY LOCATION)
find_package(Python3 COMPONENTS Interpreter Development.Module REQUIRED)

find_package(SWIG 3.0)
if(SWIG_FOUND
   AND Python3_FOUND)
  include(${SWIG_USE_FILE})
  if(Python3_FOUND)
    option(PYTHON_TESTING "Test the Python Libraries with CTest" OFF)
    execute_process(
      COMMAND ${Python3_EXECUTABLE} -m site --user-site
      OUTPUT_VARIABLE temp_PYTHON_SITE_DIRECTORY
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(PYTHON_INSTALL_DIRECTORY
        ${temp_PYTHON_SITE_DIRECTORY}
        CACHE STRING "Location where Python libraries will be installed")
    mark_as_advanced(CLEAR PYTHON_EXECUTABLE)
  else(Python3_FOUND)
    set(PYTHON_INSTALL_DIRECTORY
        "PYTHON_INSTALL_DIRECTORY-NOTFOUND"
        CACHE STRING "Location where Python libraries will be installed")
  endif(Python3_FOUND)
else()
  message(
    WARNING "SWIG/Python libraries could not be found. Python ADCIRCModules disabled.")
endif()
# ##############################################################################
