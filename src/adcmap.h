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
#ifndef ADCMOD_ADCMAP_H
#define ADCMOD_ADCMAP_H

#ifdef USE_GOOGLE_FLAT_MAP
#include "absl/container/flat_hash_map.h"
#else
#include <unordered_map>
#endif

//...Alias for using the Google hashtable if
//   available. Prevents using ifdefs throughout
//   source files
template <typename T1, typename T2>
#ifdef USE_GOOGLE_FLAT_MAP
using adcmap = absl::flat_hash_map<T1, T2>;
#else
using adcmap = std::unordered_map<T1, T2>;
#endif

#endif  // ADCMOD_ADCMAP_H
