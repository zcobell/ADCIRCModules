add_executable(
  generateAdcircSimulationDates
  ${CMAKE_CURRENT_SOURCE_DIR}/utilities/generateAdcircSimulationDates/main.cpp)
target_include_directories(
  generateAdcircSimulationDates
  PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cxxopts)
add_dependencies(generateAdcircSimulationDates adcircmodules )
target_link_libraries(generateAdcircSimulationDates
	PRIVATE adcircmodules_static ${SQLITE3_LIBRARY} HDF5::HDF5 hdf5::hdf5_hl ${NETCDF_LIBS} expat pcre dl)
install(TARGETS generateAdcircSimulationDates RUNTIME DESTINATION bin)
