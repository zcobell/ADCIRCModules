# ##############################################################################
# OpenSSL
# ##############################################################################
if(NOT "${OPENSSLHOME}" STREQUAL "")
  set(OPENSSL_ROOT_DIR "${OPENSSLHOME}")
elseif(NOT $ENV{OPENSSLHOME} STREQUAL "")
  set(OPENSSL_ROOT_DIR $ENV{OPENSSLHOME})
endif(NOT "${OPENSSLHOME}" STREQUAL "")
find_package(OpenSSL)
if(OpenSSL_FOUND)
  message(STATUS "OpenSSL libraries found. Hashing is enabled.")
else()
  message(STATUS "OpenSSL libraries not found. Hashing disabled.")
endif()
# ##############################################################################
