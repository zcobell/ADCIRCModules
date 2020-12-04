# ##############################################################################
# Boost
# ##############################################################################
find_package(Boost 1.60.0)
if(NOT Boost_FOUND OR PACKAGE_VERSION_UNSUITABLE)
  message(STATUS "Using included Boost Version: 1.66.0")
  set(Boost_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/thirdparty/boost_1_66_0)
endif()
# ##############################################################################


