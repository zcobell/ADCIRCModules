/*-------------------------------GPL-------------------------------------//
//
// QADCModules - A library for working with ADCIRC models
// Copyright (C) 2016  Zach Cobell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//-----------------------------------------------------------------------*/
#include "QADCModules.h"
#include "QADCModules_flags.h"


QADCModules::QADCModules(QObject *parent) : QObject(parent)
{
    int ierr = initialize_errors();
}

QADCModules::~QADCModules()
{

}

int QADCModules::initialize_errors()
{
    //...Generic Errors
    this->errorMapping[QADCMODULES_FILEOPENERR]     = "The specified file could not be correctly opened.";
    this->errorMapping[QADCMODULES_NULLFILENAM]     = "The filename specified is empty";
    this->errorMapping[QADCMODULES_FILENOEXIST]     = "The filename specified does not exist.";

    //...Mesh Errors
    this->errorMapping[QADCMODULES_MESHREAD_HEADER] = "There was an error while reading the header from the mesh file";
    this->errorMapping[QADCMODULES_MESHREAD_NODERR] = "There was an error while reading the nodes from the mesh file.";
    this->errorMapping[QADCMODULES_MESHREAD_ELEMER] = "There was an error while reading the elements from the mesh file.";
    this->errorMapping[QADCMODULES_MESHREAD_BNDERR] = "There was an error while reading the boundary segments from the mesh file.";
    this->errorMapping[QADCMODULES_MESHREAD_NODNUM] = "The nodes in the mesh are not sequentially numbered.";
    this->errorMapping[QADCMODULES_MESHREAD_ELENUM] = "The elements in the mesh are not sequantially numbered.";

    return 0;
}

