# ##############################################################################
# Python ADCIRCModules Library
# ##############################################################################
if(SWIG_FOUND
   AND PythonLibs_FOUND
   AND PythonInterp_FOUND)

  if(GDAL_FOUND)
    set(SWIG_GDAL_FLAG "-D_USE_GDAL")
  endif(GDAL_FOUND)

  if(PYTHONTYPE EQUAL 2)
    set(PYTHONFLAG "")
  else()
    set(PYTHONFLAG "-py3")
  endif()

  if(WIN32)

    # ...Builds a dummy cmake project Which will add some windows specific
    # include files not required by Unix/Cygwin [mingw64] Further testing
    # required to see if this is required by MSVC compilers
    file(
      WRITE ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
      "
            FILE(READ \${SRC} DATA)
            FILE(WRITE \${DST} \"#include <math.h>\")
            FILE(APPEND \${DST} \"\\n\\n\")
            FILE(APPEND \${DST} \"\${DATA}\")
        ")

    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
      COMMAND
        ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
        ${PYTHONFLAG} -I${CMAKE_CURRENT_SOURCE_DIR}/src
        -I${PYTHON_INCLUDE_PATH}
        ${SWIG_GDAL_FLAG} -o
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1
        ${CMAKE_CURRENT_SOURCE_DIR}/swig/adcirc.i
      COMMAND
        ${CMAKE_COMMAND}
        -DSRC=\"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx.1\"
        -DDST=\"${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx\" -P
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_wrapper.cmake
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating SWIG interface to Python...")

  else(WIN32)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
      COMMAND
        ${SWIG_EXECUTABLE} -outdir ${CMAKE_CURRENT_BINARY_DIR} -c++ -python
        ${PYTHONFLAG} -I${CMAKE_CURRENT_SOURCE_DIR}/src
        -I${PYTHON_INCLUDE_PATH}
        ${SWIG_GDAL_FLAG} -o
        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx
        ${CMAKE_CURRENT_SOURCE_DIR}/swig/adcirc.i
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Generating SWIG interface to Python...")
  endif(WIN32)

  add_library(pyadcircmodules SHARED
              ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/python_adcirc_wrap.cxx)
  target_include_directories(
    pyadcircmodules PRIVATE ${PYTHON_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/src)
  target_link_libraries(pyadcircmodules adcircmodules_static )
  if(OpenSSL_FOUND)
    target_include_directories(pyadcircmodules PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(pyadcircmodules ${OPENSSL_CRYPTO_LIBRARY}
                          ${OPENSSL_SSL_LIBRARY})
  endif(OpenSSL_FOUND)
  set_target_properties(pyadcircmodules PROPERTIES PREFIX "_")
  set_target_properties(pyadcircmodules PROPERTIES INSTALL_NAME_DIR "adcircmodules")
  set_property(
    DIRECTORY
    APPEND
    PROPERTY ADDITIONAL_MAKE_CLEAN_FILES pyadcircmodules.py
             CMakeFiles/python_adcirc_wrap.cxx)

  add_dependencies(pyadcircmodules adcircmodules)

  if(WIN32)
    set_target_properties(
      pyadcircmodules PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                          RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    install(
      TARGETS pyadcircmodules
      LIBRARY DESTINATION bin
      RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  elseif(CYGWIN)
    set_target_properties(pyadcircmodules PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                              ${CMAKE_CURRENT_BINARY_DIR})
    install(
      TARGETS pyadcircmodules
      LIBRARY DESTINATION lib
      RUNTIME DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  else(WIN32)
    set_target_properties(pyadcircmodules PROPERTIES LIBRARY_OUTPUT_DIRECTORY
                                              ${CMAKE_CURRENT_BINARY_DIR})
    install(TARGETS pyadcircmodules LIBRARY DESTINATION ${PYTHON_INSTALL_DIRECTORY})
  endif(WIN32)

  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/pyadcircmodules.py
          DESTINATION ${PYTHON_INSTALL_DIRECTORY})

  if(WIN32)
    set_target_properties(pyadcircmodules PROPERTIES SUFFIX ".pyd")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      # 64 bits
      target_compile_definitions(pyadcircmodules PRIVATE "MS_WIN64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
      # 32 bits
      target_compile_definitions(pyadcircmodules PRIVATE "MS_WIN32")
    endif()
  elseif(APPLE)
    set_target_properties(pyadcircmodules PROPERTIES SUFFIX ".so")
  endif(WIN32)
endif(
  SWIG_FOUND
  AND PythonLibs_FOUND
  AND PythonInterp_FOUND)
# ##############################################################################
