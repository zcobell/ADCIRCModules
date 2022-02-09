if(UNIX OR CYGWIN)
  if(BUILD_TESTS)
    # ...C++ Testing
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cxx_testcases)

    set(TEST_LIST
        cxx_readmesh.cpp
        cxx_readnetcdfmesh.cpp
        cxx_writemesh.cpp
        cxx_writeshapefile.cpp
        cxx_projectmesh.cpp
        cxx_nodalSearchTree.cpp
        cxx_elementalSearchTree.cpp
        cxx_readfort13_wmesh.cpp
        cxx_readfort13_womesh.cpp
        cxx_fort13findatt.cpp
        cxx_readasciifull.cpp
        cxx_readasciisparse.cpp
        cxx_readmaxele.cpp
        cxx_readnetcdfmaxele.cpp
        cxx_readasciivector.cpp
        cxx_readnetcdf.cpp
        cxx_readnetcdfvector.cpp
        cxx_readHarmonicsElevation.cpp
        cxx_readHarmonicsVelocity.cpp
        cxx_readnetcdfHarmonicsElevation.cpp
        cxx_readnetcdfHarmonicsVelocity.cpp
        cxx_checkmesh.cpp
        cxx_read2dm.cpp
        cxx_kdtree.cpp
        cxx_writeasciifull.cpp
        cxx_writeasciisparse.cpp
        cxx_writeasciifullvector.cpp
        cxx_writeasciisparsevector.cpp
        cxx_writenetcdf.cpp
        cxx_writenetcdfvector.cpp
        cxx_writehdf5.cpp
        cxx_makemesh.cpp
        cxx_date.cpp
        cxx_topolgy.cpp
        )

    if(ENABLE_GDAL)
      set(TEST_LIST
          ${TEST_LIST} cxx_interpolateRaster.cpp cxx_interpolateManning.cpp
          cxx_interpolateDwind.cpp cxx_writeraster.cpp)
    endif(ENABLE_GDAL)

    if(OpenSSL_FOUND)
      set(TEST_LIST ${TEST_LIST} cxx_hash.cpp cxx_hashmesh.cpp)
    endif(OpenSSL_FOUND)

    foreach(TESTFILE ${TEST_LIST})
      get_filename_component(TESTNAME ${TESTFILE} NAME_WE)
      add_executable(${TESTNAME}
                     ${CMAKE_SOURCE_DIR}/testing/cxx_tests/${TESTFILE})
      add_dependencies(${TESTNAME} adcircmodules_static)
      target_link_libraries(${TESTNAME} adcircmodules_static ${SQLITE3_LIBRARY} adcircmodules_interface ${NETCDF_LIBRARIES} HDF5::HDF5 hdf5::hdf5_hl expat pcre dl)
      target_include_directories(${TESTNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src) 
      set_target_properties(
        ${TESTNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                               ${CMAKE_BINARY_DIR}/cxx_testcases)

      add_test(
        NAME TEST_${TESTNAME}
        COMMAND ${CMAKE_BINARY_DIR}/cxx_testcases/${TESTNAME}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing)
    endforeach()

    # ...Script base testing
    add_test(
      NAME sh_interpStations
      COMMAND bash interpolate.sh
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/testing/test_scripts)

    # ...Python Test
    if(PYTHON_TESTING)
      if(PYTHONTYPE EQUAL 2)
        set(PYTHON_TESTNAME TEST_python2_adcircmodules)
        add_test(
          NAME ${PYTHON_TESTNAME}
          COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/testing/python_tests/pyadcirc_python2_testcase.py
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
      else()
        set(PYTHON_TESTNAME TEST_python3_adcircmodules)
        add_test(
          NAME ${PYTHON_TESTNAME}
          COMMAND
            ${Python3_EXECUTABLE}
            ${CMAKE_SOURCE_DIR}/testing/python_tests/pyadcirc_python3_testcase.py
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
      endif()
      set_property(
        TEST ${PYTHON_TESTNAME}
        APPEND
        PROPERTY ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}")
    endif(PYTHON_TESTING)
  endif(BUILD_TESTS)
endif(UNIX OR CYGWIN)
