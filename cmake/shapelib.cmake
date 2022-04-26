# ##############################################################################
# Shapelib Library
# ##############################################################################
add_library(
  shapelib STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shapelib/shpopen.c
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shapelib/dbfopen.c
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shapelib/safileio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shapelib/shptree.c)
target_include_directories(shapelib
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/shapelib)
# ##############################################################################
