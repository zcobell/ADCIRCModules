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
    Adcirc::Output::OutputFile *output = new Adcirc::Output::OutputFile("test_files/fort.63");

    //...Open file
    output->open();
    
    //...Read snap 1
    output->read();
    
    //...Read snap 2
    output->read();
    
    //...Read snap 3
    output->read();
    
    //...Check output
    if(output->data(2)->z(42)==0.21464678645){
        delete output;
        return 0;
    } else {
        delete output;
        return 3;
    }

}
