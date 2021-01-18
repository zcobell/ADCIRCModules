/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
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
#ifndef ADCMOD_GLOBAL_H
#define ADCMOD_GLOBAL_H

#include <cstdlib>
#include <limits>

#if defined(_MSC_VER)
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
#define DLL_EXPORT __attribute__((visibility("default")))
#define DLL_IMPORT
#if __GNUC__ > 4
#define DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define DLL_LOCAL
#endif
#elif defined(SWIG)
// When compiling the SWIG interface,
// ignore the DLL_IMPORT/DLL_EXPORT Macros
#define DLL_EXPORT
#define DLL_IMPORT
#else
#error("Don't know how to export shared object libraries")
#endif

#if defined(ADCIRCMODULES_LIBRARY)
#define ADCIRCMODULES_EXPORT DLL_EXPORT
#else
#define ADCIRCMODULES_EXPORT DLL_IMPORT
#endif
#endif  // ADCMOD_GLOBAL_H
