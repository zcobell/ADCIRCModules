# ##############################################################################
# ABSEIL SWISS TABLES
# ##############################################################################
if(NOT CYGWIN)
  option(USE_GOOGLE_FLAT_MAP "Use Google's hash table to increase speed" ON)
  if(USE_GOOGLE_FLAT_MAP)
    if(MSVC)
      # /wd4005  macro-redefinition /wd4068  unknown pragma /wd4244  conversion
      # from 'type1' to 'type2' /wd4267  conversion from 'size_t' to 'type2'
      # /wd4800  force value to bool 'true' or 'false' (performance warning)
      add_compile_options(/wd4005 /wd4068 /wd4244 /wd4267 /wd4800)
      add_definitions(/DNOMINMAX /DWIN32_LEAN_AND_MEAN=1
                      /D_CRT_SECURE_NO_WARNINGS)
    endif(MSVC)
    add_subdirectory(${CMAKE_SOURCE_DIR}/thirdparty/abseil EXCLUDE_FROM_ALL)
    message(
      STATUS
        "Using Google's Flat Map (from ABSL) instead of unordered_map for performance"
    )
  endif(USE_GOOGLE_FLAT_MAP)

mark_as_advanced( ABSL_ENABLE_INSTALL        ) 
mark_as_advanced( ABSL_GOOGLETEST_DOWNLOAD_URL)
mark_as_advanced( ABSL_LOCAL_GOOGLETEST_DIR  )
mark_as_advanced( ABSL_RUN_TESTS             )
mark_as_advanced( ABSL_USE_EXTERNAL_GOOGLETEST) 
mark_as_advanced( ABSL_USE_GOOGLETEST_HEAD    )
mark_as_advanced( BINDIR                     )

else(NOT CYGWIN)
  set(USE_GOOGLE_FLAT_MAP FALSE)
endif(NOT CYGWIN)

# ##############################################################################
