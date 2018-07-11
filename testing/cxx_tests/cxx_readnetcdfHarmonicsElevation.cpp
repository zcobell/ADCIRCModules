//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#include "adcirc.h"
#include <iostream>
#include <cmath>

int main(int argc, char *argv[]) {

  Adcirc::Output::HarmonicsOutput *harm = new Adcirc::Output::HarmonicsOutput("test_files/fort.53.nc");
  int ierr = harm->read();
  if(ierr!=Adcirc::NoError){
      delete harm;
      return ierr;
  }

  double m2_amp_value = harm->amplitude("M2")->value(0);
  double m2_pha_value = harm->phase("M2")->value(0);
  double k1_amp_value = harm->amplitude("K1")->value(50);
  double k1_pha_value = harm->phase("K1")->value(50);

  printf("%22.15f\n",m2_amp_value);
  printf("%22.15f\n",m2_pha_value);
  printf("%22.15f\n",k1_amp_value);
  printf("%22.15f\n",k1_pha_value);
  
  if(fabs(m2_amp_value-0.822651718882334)>0.000001 || fabs(m2_pha_value-247.874999341090160)>0.000001 || 
     fabs(k1_amp_value-0.092569613362145)>0.000001 || fabs(k1_pha_value-142.954998936488295)>0.000001) {
      delete harm;
      return 1;
  }

  delete harm;
  return 0;

}
