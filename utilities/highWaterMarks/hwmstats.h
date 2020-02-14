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
#ifndef HWMSTATS_H
#define HWMSTATS_H

#include "locations.h"

class HwmStats {
 public:
  HwmStats(const Locations &hwm, bool zero, double adcircMultiplier, double dataMultiplier);

  double r2() const;

  double m() const;

  double sigma() const;

  void print();

 private:
  void compute();
  Locations m_location;
  double m_r2;
  double m_m;
  double m_sigma;
  double m_b;
  double m_adcircMultiplier;
  double m_dataMultiplier;
  size_t m_ndry;
  size_t m_nwet;
  bool m_zero;
};

#endif  // HWMSTATS_H
