/*-------------------------------GPL-------------------------------------//
//
// ADCMESH - A library for working with ADCIRC models
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
#include "adcirc_errors.h"

//-----------------------------------------------------------------------------------------//
// Constructor function. Initializes the error mapping between error codes and descriptions
//-----------------------------------------------------------------------------------------//
/** \brief Constructor function for adcirc_errors class
 *
 * \author Zach Cobell
 *
 * @param parent [in] parent QObject used for memory management
 */
//-----------------------------------------------------------------------------------------//
adcirc_errors::adcirc_errors(QObject *parent) : QObject(parent)
{
    this->initializeErrors();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that generates a mapping between error codes and their descriptions that can
//   be fed to the user
//-----------------------------------------------------------------------------------------//
/** \brief Function that generates a mapping between error codes and their descriptions
 *
 * \author Zach Cobell
 *
 * This function uses a QMap to quickly turn an error code into an error description. The
 * error mapping variable maps between an integer and a QString
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_errors::initializeErrors()
{
    //...Initialize the error code
    this->errorCode = ERROR_NOERROR;

    //...No error
    this->errorMapping[ERROR_NOERROR]         = "No errors detected.";

    //...Generic Errors
    this->errorMapping[ERROR_FILEOPENERR]     = "The specified file could not be correctly opened.";
    this->errorMapping[ERROR_NULLFILENAM]     = "The filename specified is empty.";
    this->errorMapping[ERROR_FILENOEXIST]     = "The filename specified does not exist.";

    //...Mesh Read Errors
    this->errorMapping[ERROR_MESHREAD_HEADER] = "There was an error while reading the header from the mesh file";
    this->errorMapping[ERROR_MESHREAD_NODERR] = "There was an error while reading the nodes from the mesh file.";
    this->errorMapping[ERROR_MESHREAD_ELEMER] = "There was an error while reading the elements from the mesh file.";
    this->errorMapping[ERROR_MESHREAD_BNDERR] = "There was an error while reading the boundary segments from the mesh file.";
    this->errorMapping[ERROR_MESHREAD_NODNUM] = "The nodes in the mesh are not sequentially numbered.";
    this->errorMapping[ERROR_MESHREAD_ELENUM] = "The elements in the mesh are not sequantially numbered.";
    this->errorMapping[ERROR_MESHREAD_BNDUNK] = "The boundary type specified is unknown.";
    this->errorMapping[ERROR_MESHREAD_BNDNUL] = "The boundary type has not been specified.";
    this->errorMapping[ERROR_LEVEE_BELOWTOPO] = "Levee heights were specified below prevailing topography";
    this->errorMapping[ERROR_DISJOINTNODES]   = "Disjoint nodes were found";

    //...Nodal Parameter Errors
    this->errorMapping[ERROR_NODALPARAM_DEFAULTVALUESIZE] = "The size of the default value vector does not match the expected length.";
    this->errorMapping[ERROR_NODALPARAM_NOTENOUGHVALUES]  = "The size of the information read from the body is not the expected size for the nodal parameter.";
    this->errorMapping[ERROR_NODALPARAM_READERROR]        = "There was an error while attempting to read the nodal parameters.";
    this->errorMapping[ERROR_NODALPARAM_OUTOFRANGE]       = "The node number specified in the nodal parameter body is out of the acceptable range.";

    //...Nodal Attributes Errors
    this->errorMapping[ERROR_NODALATT_HEADERR]       = "There was an error reading the header of the nodal attributes file.";
    this->errorMapping[ERROR_NODALATT_BODYERR]       = "There was an error reading the body of the nodal attributes file.";
    this->errorMapping[ERROR_NODALATT_UNEXPECTEDEND] = "Unexpected end of the nodal attributes file.";

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Returns the description for an error code to the user
//-----------------------------------------------------------------------------------------//
/** \brief Return the current error code description
 *
 * \author Zach Cobell
 *
 * This function uses the current error code to generate a description for the user that
 * is meaningful and can be used to solve their issues
 **/
//-----------------------------------------------------------------------------------------//
QString adcirc_errors::getErrorString()
{
    return this->errorMapping[this->errorCode];
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Sets the current error code
//-----------------------------------------------------------------------------------------//
/** \brief Sets the current error code
 *
 * \author Zach Cobell
 *
 * @param[in] newErrorCode New error code to use
 *
 * Sets the current error code
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_errors::setError(int newErrorCode)
{
    this->errorCode = newErrorCode;
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Relays the current error code to the user
//-----------------------------------------------------------------------------------------//
/** \brief Returns the current error code
 *
 * \author Zach Cobell
 *
 * Returns the current error code
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_errors::getError()
{
    return this->errorCode;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Resets the current error to no error
//-----------------------------------------------------------------------------------------//
/** \brief Resets the current error to no error
 *
 * \author Zach Cobell
 *
 * Resets the current error to no error
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_errors::resetError()
{
    this->errorCode = ERROR_NOERROR;
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
