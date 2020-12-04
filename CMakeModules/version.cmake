# ##############################################################################
# LIBRARY VERSION
# ##############################################################################
set(ADCIRCMODULES_VERSION_MAJOR 0)
set(ADCIRCMODULES_VERSION_MINOR 3)
set(ADCIRCMODULES_VERSION_PATCH 0)
set(ADCIRCMODULES_VERSION_STRING
    ${ADCIRCMODULES_VERSION_MAJOR}.${ADCIRCMODULES_VERSION_MINOR}.${ADCIRCMODULES_VERSION_PATCH}
)
# ##############################################################################

# ##############################################################################
# CODE VERSION (GIT)
# ##############################################################################
execute_process(
  COMMAND git describe --always --tags
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
  RESULT_VARIABLE GIT_RETURN_VALUE
  OUTPUT_STRIP_TRAILING_WHITESPACE)
if(NOT "${GIT_RETURN_VALUE}" STREQUAL "0")
  set(GIT_VERSION "${ADCIRCMODULES_VERSION_STRING}.cv")
endif()
message(STATUS "ADCIRCModules Version: ${GIT_VERSION}")
# ##############################################################################
