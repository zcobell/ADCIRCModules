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
#include "adcirc_nodalattribute.h"
#include "adcirc_nodalparameter.h"


//-----------------------------------------------------------------------------------------//
// Constructor for an adcirc_nodalattribute
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_nodalattribute::adcirc_nodalattribute(adcirc_nodalparameter *nodalMetadata, QObject *parent)
 * \brief Constructor for the adcirc_nodalattribute class
 *
 * @param[in] nodalMetadata  Pointer to an adcirc_nodalattribute, which provides the metadata for this adcirc_nodalattribute
 * @param[in] parent         Pointer to a QObject. Enables automatic memory management to avoid memory leaks
 *
 * Constructs an adcirc_mesh object, takes QObject reference as input
 *
 **/
//-----------------------------------------------------------------------------------------//
adcirc_nodalattribute::adcirc_nodalattribute(adcirc_nodalparameter *nodalMetadata, QObject *parent) : QObject(parent)
{
    this->metadata = nodalMetadata;
    this->values.resize(this->metadata->nValues);
}
//-----------------------------------------------------------------------------------------//
