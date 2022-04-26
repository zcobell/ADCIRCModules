add_executable( highWaterMarks ${CMAKE_CURRENT_SOURCE_DIR}/utilities/highWaterMarks/main.cpp 
                               ${CMAKE_CURRENT_SOURCE_DIR}/utilities/highWaterMarks/highwatermarkoptions.cpp
                               ${CMAKE_CURRENT_SOURCE_DIR}/utilities/highWaterMarks/locations.cpp 
                               ${CMAKE_CURRENT_SOURCE_DIR}/utilities/highWaterMarks/hwmstats.cpp 
                               ${CMAKE_CURRENT_SOURCE_DIR}/utilities/highWaterMarks/location.cpp )
target_include_directories(
  highWaterMarks
  PRIVATE ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/src
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cxxopts 
          ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shapelib )
add_dependencies( highWaterMarks adcircmodules shapelib )
if(ADCIRCMODULES_ENABLE_STACK_TRACE)
  target_compile_definitions( highWaterMarks
                             PRIVATE ADCIRCMODULES_FORCE_SANITIZER_STACK_TRACE)
endif()
target_link_libraries( highWaterMarks PRIVATE adcircmodules shapelib)
install(TARGETS highWaterMarks RUNTIME DESTINATION bin)
