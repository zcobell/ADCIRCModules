add_executable(
  generateAdcircSimulationDates
  ${CMAKE_CURRENT_SOURCE_DIR}/utilities/generateAdcircSimulationDates/main.cpp)
target_include_directories(
  generateAdcircSimulationDates
  PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cxxopts)
add_dependencies(generateAdcircSimulationDates adcircmodules )
target_link_libraries(generateAdcircSimulationDates
	PRIVATE adcircmodules adcircmodules_static)
install(TARGETS generateAdcircSimulationDates RUNTIME DESTINATION bin)
