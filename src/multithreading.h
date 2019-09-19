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
#ifndef ADCMOD_MULTITHREADING_H
#define ADCMOD_MULTITHREADING_H
#include "adcircmodules_global.h"
namespace Adcirc {

class Multithreading {
 public:
  Multithreading() = default;

  static void ADCIRCMODULES_EXPORT disable();
  static void ADCIRCMODULES_EXPORT enable(int numThreads = 0);
  static int ADCIRCMODULES_EXPORT numThreads();
  static int ADCIRCMODULES_EXPORT maxThreads();
  static void ADCIRCMODULES_EXPORT setNumThreads(int numThreads);
  static void ADCIRCMODULES_EXPORT setMaximumThreads();
};
}  // namespace Adcirc

#endif  // ADCMOD_MULTITHREADING_H
