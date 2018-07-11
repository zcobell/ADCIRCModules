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

int main(int argc, char *argv[]) {

  Adcirc::ModelParameters::NodalAttributes *fort13 = new Adcirc::ModelParameters::NodalAttributes("test_files/ms-riv.13");
  fort13->read();

  double manning_value = fort13->attribute("mannings_n_at_sea_floor",0)->value(0);
  if(manning_value != 0.012){
      delete fort13;
      return 1;
  }
  else {
      delete fort13;
      return 0;
  }

}
