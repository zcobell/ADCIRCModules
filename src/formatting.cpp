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
#include "formatting.h"

#include "boost/format.hpp"

static boost::format c_adcircAsciiFileHeader(
    "%6i %10i %10.6f %6i %6i FileFmtVersion: %10i\n");
static boost::format c_adcircAsciiScalar("%8i     %20.10e\n");
static boost::format c_adcircAsciiVector("%8i     %20.10e     %20.10e\n");
static boost::format c_adcircAscii3d("%8i     %20.10e     %20.10e     %20.10e\n");
static boost::format c_adcircRecordHeaderSparse(
    "%20.10e     %10i  %10i %20.10e\n");
static boost::format c_adcircRecordHeaderFull("%20.10e     %10i\n");

std::string Adcirc::Output::Formatting::adcircFileHeader(
    const size_t numSnaps, const size_t numNodes, const double dt,
    const long long dIter, const size_t dimension) {
  constexpr size_t fileformat = 1050624;
  return boost::str(c_adcircAsciiFileHeader % numSnaps % numNodes % dt % dIter %
                    dimension % fileformat);
}

std::string Adcirc::Output::Formatting::adcircFullFormatRecordHeader(
    const double time, const long long iteration) {
  return boost::str(c_adcircRecordHeaderFull % time % iteration);
}

std::string Adcirc::Output::Formatting::adcircSparseFormatRecordHeader(
    const double time, const long long iterations, const size_t numNonDefault,
    const double defaultValue) {
  return boost::str(c_adcircRecordHeaderSparse % time % iterations %
                    numNonDefault % defaultValue);
}

std::string Adcirc::Output::Formatting::adcircScalarLineFormat(
    const size_t id, const double value) {
  return boost::str(c_adcircAsciiScalar % id % value);
}

std::string Adcirc::Output::Formatting::adcircVectorLineFormat(
    const size_t id, const double value1, const double value2) {
  return boost::str(c_adcircAsciiVector % id % value1 % value2);
}

std::string Adcirc::Output::Formatting::adcirc3dLineFormat(
    const size_t id, const double value1, const double value2,
    const double value3) {
  return boost::str(c_adcircAscii3d % id % value1 % value2 % value3);
}
