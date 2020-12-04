add_executable(interpolateAdcircStations
               ${CMAKE_CURRENT_SOURCE_DIR}/utilities/interpolateAdcircStations/main.cpp)
target_include_directories(
  interpolateAdcircStations
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/utilities/interpolateAdcircStations
          ${Boost_INCLUDE_DIRS}
          ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cxxopts)
add_dependencies(interpolateAdcircStations adcircmodules_static )
target_link_libraries(interpolateAdcircStations PRIVATE adcircmodules_static)
install(TARGETS interpolateAdcircStations RUNTIME DESTINATION bin)
