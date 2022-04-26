# ##############################################################################
# C++ 14 Check
# ##############################################################################
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++14" COMPILER_SUPPORTS_CXX14)
if(COMPILER_SUPPORTS_CXX14)
  set(CMAKE_CXX_STANDARD 14)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
else(COMPILER_SUPPORTS_CXX14)
  message(
    FATAL_ERROR
      "The compiler ${CMAKE_CXX_COMPILER} has no C++14 support. Please use a different C++ compiler."
  )
endif(COMPILER_SUPPORTS_CXX14)
# ##############################################################################
