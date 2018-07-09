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
    Adcirc::Output::OutputFile *output = new Adcirc::Output::OutputFile("test_files/fort.64.nc");
    output->open();
    output->read();
    output->read();
    output->read();
    std::cout << "Expected: -0.000205562, Got: " << output->data(2)->u(925) << std::endl;
    if(fabs(output->data(2)->u(925)-(-0.000205562))>0.000001){
        delete output;
        return 1;
    }
    delete output;
    return 0;
}
