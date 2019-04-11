/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
//
// ADCIRCModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ADCIRCModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------*/
#include "multithreading.h"
#include "logging.h"

#ifdef _OPENMP
#include <omp.h>
#endif

void Multithreading::enable(int numThreads) {
  if (numThreads == 0) {
    Multithreading::setMaximumThreads();
  } else {
    Multithreading::setNumThreads(numThreads);
  }
  return;
}

void Multithreading::disable() { Multithreading::setNumThreads(1); }

int Multithreading::maxThreads() {
#ifndef _OPENMP
  Adcirc::Logging::warning("Multithreading is not enabled.");
  return 1;
#else
  return omp_get_num_procs();
#endif
}

int Multithreading::numThreads() {
#ifndef _OPENMP
  Adcirc::Logging::warning("Multithreading is not enabled.");
  return 1;
#else
  return omp_get_max_threads();
#endif
}

void Multithreading::setNumThreads(int numThreads) {
#ifndef _OPENMP
  Adcirc::Logging::warning("Multithreading is not enabled.");
#else
  if (numThreads > omp_get_num_procs()) {
    Adcirc::Logging::warning(
        "Specified number of threads > maximum available threads.");
  } else {
    omp_set_num_threads(numThreads);
  }
#endif
  return;
}

void Multithreading::setMaximumThreads() {
#ifndef _OPENMP
  Adcirc::Logging::warning("Multithreading is not enabled.");
#else
  omp_set_num_threads(omp_get_num_procs());
#endif
  return;
}
