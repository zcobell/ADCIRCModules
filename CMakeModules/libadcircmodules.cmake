# ##############################################################################
# ADCIRCModules Sources
# ##############################################################################
set(ADCIRCMODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FileTypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Formatting.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdcHash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hmdf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HmdfStation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StationInterpolationOptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StationInterpolation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NetcdfTimeseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdcHashPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CDate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Boundary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FileIO.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StringConversion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NodalAttributes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AttributeMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OutputMetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OutputRecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ReadOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WriteOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HarmonicsRecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HarmonicsOutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ElementTable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NodeTable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Meshchecker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Multithreading.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Constants.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MeshPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Projection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/KDTree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/KDTreePrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Topology.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FaceTable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ProgressBar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HarmonicsOutputPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HarmonicsRecordPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NodalAttributesPrivate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OceanweatherHeader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OceanweatherRecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Oceanweather.cpp)
if(GDAL_FOUND)
  set(ADCIRCMODULES_SOURCES
      ${ADCIRCMODULES_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/Griddata.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/GriddataPrivate.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/Pixel.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/RasterData.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataAverage.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataNearest.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataHighest.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataStandardDeviation.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataBilskie.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataInverseDistanceWeighted.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataInverseDistanceWeightedNPoints.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataAverageNearestNPoints.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataWindRoughness.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolation/GriddataMethod.cpp ../src/OceanweatherTrackInfo.h)
endif(GDAL_FOUND)

set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdcircModules.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdcircModules_Global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AttributeMetadata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Boundary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AdcHash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HashType.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Hmdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HmdfStation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StationInterpolationOptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/StationInterpolation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FileTypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HarmonicsOutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/HarmonicsRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/InterpolationMethods.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FileIO.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Projection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NodalAttributes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ReadOutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/WriteOutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OutputRecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OutputMetadata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Meshchecker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ElementTable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NodeTable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/FaceTable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Topology.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Face.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Multithreading.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/KDTree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/DefaultValues.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ProgressBar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/CDate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Point.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Oceanweather.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OceanweatherHeader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/OceanweatherRecord.h)
# ##############################################################################

add_library(adcircmodules_interface INTERFACE )
add_library(adcircmodules_objectlib OBJECT ${ADCIRCMODULES_SOURCES})
add_library(adcircmodules_static STATIC $<TARGET_OBJECTS:adcircmodules_objectlib>)
add_library(adcircmodules SHARED $<TARGET_OBJECTS:adcircmodules_objectlib>)

set_property(TARGET adcircmodules_objectlib PROPERTY POSITION_INDEPENDENT_CODE 1)
target_link_libraries(adcircmodules_static adcircmodules_interface)
target_link_libraries(adcircmodules adcircmodules_interface)

if(ADCIRCMODULES_USE_FAST_MATH)
    message(STATUS "ADCIRCModules will be built with math approximations")
    target_compile_definitions(adcircmodules_objectlib PRIVATE ADCMOD_USE_FAST_MATH)
endif()

if(NOT PROJ_FOUND)
	add_dependencies(adcircmodules_interface shapelib adcmod_proj generate_proj_db)
	target_compile_definitions(adcircmodules_objectlib PRIVATE USE_INTERNAL_PROJ)
else()
	add_dependencies(adcircmodules_interface shapelib)
endif()

set( adcircmodules_include_list 
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/src/interpolation
            ${CMAKE_SOURCE_DIR}/thirdparty/shapelib
            ${CMAKE_SOURCE_DIR}/thirdparty/date/include/date
            ${CMAKE_SOURCE_DIR}/thirdparty/nanoflann/include
            ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
            ${CMAKE_SOURCE_DIR}/thirdparty/indicators
            ${Boost_INCLUDE_DIRS}
			${PROJ_INCLUDE_DIR}
            ${SQLite3_INCLUDE_DIR}
            ${NETCDF_INCLUDE_DIR})

target_include_directories(adcircmodules_objectlib PRIVATE ${adcircmodules_include_list})
target_compile_definitions(adcircmodules_objectlib PRIVATE GIT_VERSION="${GIT_VERSION}")
target_compile_definitions(adcircmodules_objectlib PRIVATE ADCIRCMODULES_LIBRARY)

if(GDAL_FOUND)
  target_compile_definitions(adcircmodules_objectlib PRIVATE "USE_GDAL")
  target_include_directories(adcircmodules_objectlib PRIVATE ${GDAL_INCLUDE_DIR})
  link_directories(${GDAL_LIBPATH})
  target_link_libraries(adcircmodules_interface INTERFACE ${GDAL_LIBRARY})
  set(HEADER_LIST ${HEADER_LIST} ${CMAKE_SOURCE_DIR}/src/RasterData.h
                  ${CMAKE_SOURCE_DIR}/src/Griddata.h)
endif(GDAL_FOUND)

set_target_properties(
  adcircmodules PROPERTIES VERSION ${ADCIRCMODULES_VERSION_STRING}
                           SOVERSION ${ADCIRCMODULES_VERSION_MAJOR})
set_target_properties(adcircmodules PROPERTIES PUBLIC_HEADER "${HEADER_LIST}")
if(${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local"
   OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/local/"
   OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/"
   OR ${CMAKE_INSTALL_PREFIX} STREQUAL "/usr/")
  set(HEADER_DEST "${CMAKE_INSTALL_INCLUDEDIR}/adcircmodules")
else()
  set(HEADER_DEST ${CMAKE_INSTALL_INCLUDEDIR})
endif()

write_basic_package_version_file(
  adcircmodulesConfigVersion.cmake
  VERSION ${ADCIRCMODULES_VERSION_STRING}
  COMPATIBILITY SameMajorVersion)

install(
  TARGETS adcircmodules
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ADCIRCMODULES_RUNTIME
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT ADCIRCMODULES_RUNTIME
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          COMPONENT ADCIRCMODULES_DEVELOPMENT
  PUBLIC_HEADER DESTINATION ${HEADER_DEST} COMPONENT ADCIRCMODULES_DEVELOPMENT)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/adcircmodulesConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

set_target_properties(adcircmodules_objectlib PROPERTIES CMAKE_CXX_VISIBILITY_PRESET
                                               hidden)
set_target_properties(adcircmodules_objectlib PROPERTIES CMAKE_CXX_INLINES_HIDDEN YES)

target_link_libraries(adcircmodules_interface INTERFACE shapelib ${SQLite3_LIBRARY})

if(WIN32)
  link_directories(${CMAKE_SOURCE_DIR}/thirdparty/netcdf/libs_vc64)
  target_link_libraries(adcircmodules_interface INTERFACE netcdf hdf5 hdf5_hl)
else(WIN32)
  target_link_libraries(adcircmodules_interface INTERFACE ${NETCDF_LIBRARIES} ${PROJ_LIBRARY})
endif(WIN32)

if(OpenSSL_FOUND)
  target_compile_definitions(adcircmodules_objectlib PRIVATE ADCMOD_HAVE_OPENSSL)
  target_include_directories(adcircmodules_objectlib PRIVATE ${OPENSSL_INCLUDE_DIR})
  target_link_libraries(adcircmodules_interface INTERFACE ${OPENSSL_CRYPTO_LIBRARY}
                        ${OPENSSL_SSL_LIBRARY})
endif(OpenSSL_FOUND)

if(APPLE)
  set(CMAKE_MACOSX_RPATH 0)
  set_target_properties(adcircmodules PROPERTIES INSTALL_NAME_DIR
                                                 "adcircmodules")
  set_target_properties(adcircmodules PROPERTIES MACOSX_RPATH "adcircmodules")
endif(APPLE)

if(OPENMP_FOUND)
  target_compile_options(adcircmodules_objectlib PRIVATE ${OpenMP_CXX_FLAGS})
  target_link_libraries(adcircmodules_interface INTERFACE ${OpenMP_CXX_LIB_NAMES}
                        ${OpenMP_CXX_FLAGS})
endif(OPENMP_FOUND)

if(HDF5_FOUND)
  target_compile_definitions(adcircmodules_objectlib PRIVATE HAVE_HDF5
                                                   ${HDF5_DEFINITIONS})
  target_include_directories(adcircmodules_objectlib PRIVATE ${HDF5_INCLUDE_DIRS})
  target_link_libraries(adcircmodules_interface INTERFACE ${HDF5_LIBRARIES})
endif(HDF5_FOUND)
