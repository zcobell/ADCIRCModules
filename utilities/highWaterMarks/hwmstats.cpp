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
#include "hwmstats.h"
#include <cmath>
#include <iostream>
#include "boost/format.hpp"

HwmStats::HwmStats(const Locations &hwm, bool zero)
    : m_nwet(0),
      m_ndry(0),
      m_r2(0),
      m_m(0),
      m_b(0),
      m_sigma(0),
      m_zero(zero),
      m_location(hwm) {
  this->compute();
}

double HwmStats::r2() const { return m_r2; }

double HwmStats::m() const { return m_m; }

double HwmStats::sigma() const { return m_sigma; }

void HwmStats::compute() {
  double sumxy = 0;
  double sumx2 = 0;
  double sumy2 = 0;
  double sumy = 0;
  double sumx = 0;
  double sumErr = 0;

  for (size_t i = 0; i < this->m_location.size(); ++i) {
    if (this->m_location.location(i)->modeled() > -900) {
      sumx += this->m_location.location(i)->measured();
      sumy += this->m_location.location(i)->modeled();
      sumxy += this->m_location.location(i)->measured() *
               this->m_location.location(i)->modeled();
      sumx2 += std::pow(this->m_location.location(i)->measured(), 2.0);
      sumy2 += std::pow(this->m_location.location(i)->modeled(), 2.0);
      sumErr += this->m_location.location(i)->difference();
      this->m_nwet++;
    } else {
      this->m_ndry++;
    }
  }

  if (this->m_zero) {
    this->m_b = 0;
    this->m_m = sumxy / sumx2;
    double ybar = sumy / static_cast<double>(this->m_nwet);
    double sstot = 0;
    for (size_t i = 0; i < this->m_location.size(); ++i) {
      if (this->m_location.location(i)->modeled() > -900) {
        sstot += std::pow(this->m_location.location(i)->modeled() - ybar, 2.0);
      }
    }
    double sse = sumy2 - std::pow(this->m_m, 2.0) * sumx2;
    this->m_r2 = 1.0 - (sse / sstot);
  } else {
    this->m_m = (this->m_nwet * sumxy - sumx * sumy) /
                (this->m_nwet * sumx2 - (sumx * sumx));
    this->m_b = ((sumy * sumx2) - (sumx * sumxy)) /
                (this->m_nwet * sumx2 - (sumx * sumx));
    this->m_r2 = pow(((this->m_nwet * sumxy - (sumx * sumy)) /
                      sqrt((this->m_nwet * sumx2 - (sumx * sumx)) *
                           (this->m_nwet * sumy2 - (sumy * sumy)))),
                     2.0);
  }

  double meanerr = sumErr / this->m_nwet;
  double sumSqerr = 0;
  for (size_t i = 0; i < this->m_location.size(); ++i) {
    if (this->m_location.location(i)->modeled() > -900) {
      sumSqerr +=
          std::pow(this->m_location.location(i)->difference() - meanerr, 2.0);
    }
  }
  this->m_sigma = std::sqrt(sumSqerr / this->m_nwet);

  return;
}

void HwmStats::print() {
  std::cout << "Statistics:" << std::endl;
  std::cout << "      slope: " << boost::str(boost::format("%5.2f") % this->m_m)
            << std::endl;
  if (!this->m_zero) {
    std::cout << "  intercept: "
              << boost::str(boost::format("%5.2f") % this->m_b) << std::endl;
  }
  std::cout << "         r2: "
            << boost::str(boost::format("%5.2f") % this->m_r2) << std::endl;
  std::cout << "      sigma: "
            << boost::str(boost::format("%5.2f") % this->m_sigma) << std::endl;
  return;
}
