#include "adcirc_errors.h"

adcirc_errors::adcirc_errors(QObject *parent) : QObject(parent)
{
    this->initializeErrors();
}


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
    this->errorCode = ADCMESH_NOERROR;

    //...No error
    this->errorMapping[ADCMESH_NOERROR]         = "No errors detected.";

    //...Generic Errors
    this->errorMapping[ADCMESH_FILEOPENERR]     = "The specified file could not be correctly opened.";
    this->errorMapping[ADCMESH_NULLFILENAM]     = "The filename specified is empty";
    this->errorMapping[ADCMESH_FILENOEXIST]     = "The filename specified does not exist.";

    //...Mesh Read Errors
    this->errorMapping[ADCMESH_MESHREAD_HEADER] = "There was an error while reading the header from the mesh file";
    this->errorMapping[ADCMESH_MESHREAD_NODERR] = "There was an error while reading the nodes from the mesh file.";
    this->errorMapping[ADCMESH_MESHREAD_ELEMER] = "There was an error while reading the elements from the mesh file.";
    this->errorMapping[ADCMESH_MESHREAD_BNDERR] = "There was an error while reading the boundary segments from the mesh file.";
    this->errorMapping[ADCMESH_MESHREAD_NODNUM] = "The nodes in the mesh are not sequentially numbered.";
    this->errorMapping[ADCMESH_MESHREAD_ELENUM] = "The elements in the mesh are not sequantially numbered.";
    this->errorMapping[ADCMESH_MESHREAD_BNDUNK] = "The boundary type specified is unknown";
    this->errorMapping[ADCMESH_MESHREAD_BNDNUL] = "The boundary type has not been specified.";

    return ADCMESH_NOERROR;
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
