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
#ifndef ADCMOD_HARMONICSRECORDIMPL_H
#define ADCMOD_HARMONICSRECORDIMPL_H

#include <string>
#include <vector>

class HarmonicsRecordImpl {
 public:
  HarmonicsRecordImpl();

  std::string name() const;
  void setName(const std::string& name);

  double value(size_t index);
  std::vector<double> values();

  void set(size_t index, double value);
  void set(const std::vector<double>& value);

  void resize(size_t numNodes);

  double frequency() const;
  void setFrequency(double frequency);

  double nodalFactor() const;
  void setNodalFactor(double nodalFactor);

  double equilibriumArg() const;
  void setEquilibriumArg(double equilibriumArg);

 private:
  size_t m_numNodes;
  std::string m_name;
  std::vector<double> m_data;
  double m_frequency;
  double m_nodalFactor;
  double m_equilibriumArg;
};

#endif  // ADCMOD_HARMONICSRECORD_H
