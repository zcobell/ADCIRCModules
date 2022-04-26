# ##############################################################################
# Boost
# ##############################################################################
find_package(Boost 1.75.0)
if(NOT Boost_FOUND OR PACKAGE_VERSION_UNSUITABLE)
  message(STATUS "Using included Boost Version: 1.75.0")
  set(Boost_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_75_0)
endif()
# ##############################################################################


