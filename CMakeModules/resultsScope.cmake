add_executable(resultsScope ${CMAKE_CURRENT_SOURCE_DIR}/utilities/resultsScope/main.cpp)
target_include_directories(
  resultsScope
  PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cxxopts)
add_dependencies(resultsScope adcircmodules )
target_link_libraries(resultsScope PRIVATE adcircmodules_static ${SQLITE3_LIBRARY} ${Boost_LIBRARIES} ${NETCDF_LIBRARIES} HDF5::HDF5 hdf5::hdf5_hl expat pcre dl)
install(TARGETS resultsScope RUNTIME DESTINATION bin)
