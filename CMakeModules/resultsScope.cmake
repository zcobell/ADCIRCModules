add_executable(resultsScope ${CMAKE_CURRENT_SOURCE_DIR}/utilities/resultsScope/main.cpp)
target_include_directories(
  resultsScope
  PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cxxopts)
add_dependencies(resultsScope adcircmodules )
if(ADCIRCMODULES_ENABLE_STACK_TRACE)
  target_compile_definitions( resultsScope
                             PRIVATE ADCIRCMODULES_FORCE_SANITIZER_STACK_TRACE)
endif()
target_link_libraries(resultsScope PRIVATE adcircmodules ${Boost_LIBRARIES})
install(TARGETS resultsScope RUNTIME DESTINATION bin)
