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
#include "adcircmodules_global.h"

namespace Adcirc {

namespace FileIO {

namespace Generic {
void readFileData(const std::string &filename, std::vector<std::string> &data);

void splitString(std::string &data, std::vector<std::string> &result);

std::string getFileExtension(const std::string &filename);

bool fileExists(const std::string &filename);

}  // namespace Generic

namespace AdcircIO {
bool splitStringNodeFormat(std::string &data, size_t &id, double &x, double &y,
                           double &z);

bool splitStringElemFormat(std::string &data, size_t &id,
                           std::vector<size_t> &nodes);

bool splitStringBoundary0Format(std::string &data, size_t &node1);

bool splitStringBoundary23Format(std::string &data, size_t &node1,
                                 double &crest, double &supercritical);

bool splitStringBoundary24Format(std::string &data, size_t &node1,
                                 size_t &node2, double &crest,
                                 double &subcritical, double &supercritical);

bool splitStringBoundary25Format(std::string &data, size_t &node1,
                                 size_t &node2, double &crest,
                                 double &subcritical, double &supercritical,
                                 double &pipeheight, double &pipecoef,
                                 double &pipediam);

bool splitStringAttribute1Format(std::string &data, size_t &node,
                                 double &value);

bool splitStringAttribute2Format(std::string &data, size_t &node,
                                 double &value1, double &value2);

bool splitStringAttributeNFormat(std::string &data, size_t &node,
                                 std::vector<double> &values);

bool splitStringHarmonicsElevationFormat(std::string &data, double &amplitude,
                                         double &phase);

bool splitStringHarmonicsVelocityFormat(std::string &data, double &u_magnitude,
                                        double &u_phase, double &v_magnitude,
                                        double &v_phase);
}  // namespace AdcircIO
namespace SMSIO {

bool splitString2dmNodeFormat(std::string &data, size_t &id, double &x,
                              double &y, double &z);

bool splitString2dmElementFormat(std::string &data, size_t &id,
                                 std::vector<size_t> &nodes);
}  // namespace SMSIO

}  // namespace FileIO

}  // namespace Adcirc

#endif  // ADCMOD_FILEIO_H
