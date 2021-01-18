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
#ifndef ADCMOD_FILEIO_H
#define ADCMOD_FILEIO_H

#include <string>
#include <vector>

#include "AdcircModules_Global.h"

namespace Adcirc {

namespace FileIO {

namespace Generic {
void ADCIRCMODULES_EXPORT readFileData(const std::string &filename,
                                       std::vector<std::string> &data);

void ADCIRCMODULES_EXPORT splitString(std::string &data,
                                      std::vector<std::string> &result);

std::string ADCIRCMODULES_EXPORT getFileExtension(const std::string &filename);

std::string ADCIRCMODULES_EXPORT
getFileWithoutExtension(const std::string &filename);

bool ADCIRCMODULES_EXPORT fileExists(const std::string &filename);

std::string ADCIRCMODULES_EXPORT sanitizeString(const std::string &a);

}  // namespace Generic

namespace AdcircIO {
bool ADCIRCMODULES_EXPORT splitStringNodeFormat(const std::string &data,
                                                size_t &id, double &x,
                                                double &y, double &z);

bool ADCIRCMODULES_EXPORT splitStringElemFormat(const std::string &data,
                                                size_t &id,
                                                std::vector<size_t> &nodes);

bool ADCIRCMODULES_EXPORT splitStringBoundary0Format(const std::string &data,
                                                     size_t &node1);

bool ADCIRCMODULES_EXPORT splitStringBoundary23Format(const std::string &data,
                                                      size_t &node1,
                                                      double &crest,
                                                      double &supercritical);

bool ADCIRCMODULES_EXPORT splitStringBoundary24Format(
    const std::string &data, size_t &node1, size_t &node2, double &crest,
    double &subcritical, double &supercritical);

bool ADCIRCMODULES_EXPORT splitStringBoundary25Format(
    const std::string &data, size_t &node1, size_t &node2, double &crest,
    double &subcritical, double &supercritical, double &pipeheight,
    double &pipecoef, double &pipediam);

bool ADCIRCMODULES_EXPORT splitStringAttribute1Format(const std::string &data,
                                                      size_t &node,
                                                      double &value);

bool ADCIRCMODULES_EXPORT splitStringAttribute2Format(const std::string &data,
                                                      size_t &node,
                                                      double &value1,
                                                      double &value2);

bool ADCIRCMODULES_EXPORT splitStringAttributeNFormat(
    const std::string &data, size_t &node, std::vector<double> &values);

bool ADCIRCMODULES_EXPORT splitStringHarmonicsElevationFormat(
    const std::string &data, double &amplitude, double &phase);

bool ADCIRCMODULES_EXPORT splitStringHarmonicsVelocityFormat(
    const std::string &data, double &u_magnitude, double &u_phase,
    double &v_magnitude, double &v_phase);
}  // namespace AdcircIO
namespace SMSIO {

bool ADCIRCMODULES_EXPORT splitString2dmNodeFormat(const std::string &data,
                                                   size_t &id, double &x,
                                                   double &y, double &z);

bool ADCIRCMODULES_EXPORT splitString2dmElementFormat(
    const std::string &data, size_t &id, std::vector<size_t> &nodes);
}  // namespace SMSIO

namespace HMDFIO {
bool ADCIRCMODULES_EXPORT splitStringHmdfFormat(const std::string &data,
                                                int &year, int &month, int &day,
                                                int &hour, int &minute,
                                                int &second, double &value);
}

}  // namespace FileIO

}  // namespace Adcirc

#endif  // ADCMOD_FILEIO_H
