# ##############################################################################
# ADCIRCModules Sources
# ##############################################################################
set(ADCIRCMODULES_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filetypes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/formatting.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/node.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/element.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adchash.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hmdf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hmdfstation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stationinterpolationoptions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stationinterpolation.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/netcdftimeseries.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adchash_private.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cdate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boundary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fileio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stringconversion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nodalattributes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/attribute.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/attributemetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/outputmetadata.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/outputrecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/readoutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/writeoutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/harmonicsrecord.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/harmonicsoutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/elementtable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nodetable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/meshchecker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/multithreading.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh_private.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/projection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/kdtree.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/kdtree_private.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/topology.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/facetable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/harmonicsoutput_private.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/harmonicsrecord_private.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nodalattributes_private.cpp)
if(GDAL_FOUND)
  set(ADCIRCMODULES_SOURCES
      ${ADCIRCMODULES_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/src/griddata.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/griddata_private.cpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/pixel.cpp ${CMAKE_CURRENT_SOURCE_DIR}/src/rasterdata.cpp)
endif(GDAL_FOUND)

set(HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adcircmodules.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adcircmodules_global.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/attribute.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/attributemetadata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/boundary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/config.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/element.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adchash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hashtype.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hmdf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hmdfstation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stationinterpolationoptions.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/stationinterpolation.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/filetypes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/harmonicsoutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/harmonicsrecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/interpolationmethods.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fileio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/projection.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nodalattributes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/node.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/readoutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/writeoutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/outputrecord.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/outputmetadata.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/meshchecker.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/elementtable.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/multithreading.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/constants.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/kdtree.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/default_values.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/cdate.h)
# ##############################################################################

add_library(adcircmodules_interface INTERFACE )
add_library(adcircmodules_objectlib OBJECT ${ADCIRCMODULES_SOURCES})
add_library(adcircmodules_static STATIC $<TARGET_OBJECTS:adcircmodules_objectlib>)
add_library(adcircmodules SHARED $<TARGET_OBJECTS:adcircmodules_objectlib>)

set_property(TARGET adcircmodules_objectlib PROPERTY POSITION_INDEPENDENT_CODE 1)
target_link_libraries(adcircmodules_static adcircmodules_interface)
target_link_libraries(adcircmodules adcircmodules_interface)

if(GDAL_FOUND)
  target_compile_definitions(adcircmodules_objectlib PRIVATE "USE_GDAL")
  target_include_directories(adcircmodules_objectlib PRIVATE ${GDAL_INCLUDE_DIR})
  link_directories(${GDAL_LIBPATH})
  target_link_libraries(adcircmodules_interface INTERFACE ${GDAL_LIBRARY})
  set(HEADER_LIST ${HEADER_LIST} ${CMAKE_SOURCE_DIR}/src/rasterdata.h
                  ${CMAKE_SOURCE_DIR}/src/griddata.h)
endif(GDAL_FOUND)

if(NOT PROJ_FOUND)
	add_dependencies(adcircmodules_interface shapelib adcmod_proj generate_proj_db)
	target_compile_definitions(adcircmodules_objectlib PRIVATE USE_INTERNAL_PROJ)
else()
	add_dependencies(adcircmodules_interface shapelib)
endif()

set( adcircmodules_include_list 
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_SOURCE_DIR}/thirdparty/shapelib
            ${CMAKE_SOURCE_DIR}/thirdparty/abseil
            ${CMAKE_SOURCE_DIR}/thirdparty/date/include/date
            ${CMAKE_SOURCE_DIR}/thirdparty/nanoflann/include
            ${CMAKE_SOURCE_DIR}/thirdparty/cxxopts
			${PROJ_INCLUDE_DIR}
            ${Boost_INCLUDE_DIRS}
            ${NETCDF_INCLUDE_DIR})

target_include_directories(adcircmodules_objectlib PRIVATE ${adcircmodules_include_list})
target_compile_definitions(adcircmodules_objectlib PRIVATE GIT_VERSION="${GIT_VERSION}")
target_compile_definitions(adcircmodules_objectlib PRIVATE ADCIRCMODULES_LIBRARY)

set_target_properties(
  adcircmodules PROPERTIES VERSION ${ADCIRCMODULES_VERSION_STRING}
                           SOVERSION ${ADCIRCMODULES_VERSION_MAJOR})

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

target_link_libraries(adcircmodules_interface INTERFACE shapelib)

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

if(USE_GOOGLE_FLAT_MAP)
  target_link_libraries(adcircmodules_interface INTERFACE absl::flat_hash_map)
  add_dependencies(adcircmodules_interface absl::flat_hash_map)
  target_compile_definitions(adcircmodules_objectlib PRIVATE USE_GOOGLE_FLAT_MAP)
endif(USE_GOOGLE_FLAT_MAP)

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
