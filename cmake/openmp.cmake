# ##############################################################################
# OpenMP
# ##############################################################################
option(ENABLE_OPENMP "Enable OpenMP Multithreading" ON)
if(ENABLE_OPENMP)
  find_package(OpenMP)
else(ENABLE_OPENMP)

endif(ENABLE_OPENMP)
# ##############################################################################
