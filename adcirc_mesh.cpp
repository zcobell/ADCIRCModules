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
#include "adcirc_mesh.h"
#include <QDebug>

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor
 *
 * \author Zach Cobell
 *
 *  Constructs an adcirc_mesh object, takes QObject reference as input
 *
 * @param *parent [in] reference to QObject. Enables automatic memory management to avoid memory leaks
 *
 **/
adcirc_mesh::adcirc_mesh(QObject *parent) : QObject(parent)
{
    //...Initialize the errors so we can
    //   describe what went wrong to the
    //   user later
    this->initializeErrors();

    //...By default, we will assume that the mesh numbering should be sequential
    this->ignoreMeshNumbering = false;
    this->meshNeedsNumbering  = false;

    return;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//
//
//   P U B L I C
//             F U N C T I O N S
//
//
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Returns the error code variable to the user
//-----------------------------------------------------------------------------------------//
/** \brief Return the current error code from the class
 *
 * \author Zach Cobell
 *
 * This function returns the current error code from the class to the user. The function
 * adcirc_mesh::getErrorString() would likely do a better job describing the error, however
 * this function can be used to check if things have completed successfully or not
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::getErrorCode()
{
    return this->errorCode;
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
QString adcirc_mesh::getErrorString()
{
    return this->errorMapping[this->errorCode];
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Public function to read an ADCIRC mesh. Assumes filename already specified
//-----------------------------------------------------------------------------------------//
/** \brief Read an ADCIRC mesh into the class
 *
 * \author Zach Cobell
 *
 * Function used to read the ADCIRC mesh into the class. Assumes the filename has already
 * been specified. The mesh file will be read in after checking for some simple errors. Error codes
 * will be returned upon any error. Any return besides ADCMESH_NOERROR is a fatal
 * error
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::read()
{
    //...Check for a null string
    if(this->filename==QString())
    {
        this->errorCode = ADCMESH_NULLFILENAM;
        return this->errorCode;
    }

    //...Check for file exists
    QFile thisFile(this->filename);
    if(!thisFile.exists())
    {
        this->errorCode = ADCMESH_FILENOEXIST;
        return this->errorCode;
    }

    //...Assuming these two checks passed, we can call
    //   the main routine
    int ierr = this->readMesh();

    return ierr;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Public function to read an ADCIRC mesh. Assumes filename specified in input
//-----------------------------------------------------------------------------------------//
/** \brief  Read an ADCIRC mesh into the class and set the filename
 *
 *  \author Zach Cobell
 *
 *   @param inputFile [in] specifies the mesh file to be read
 *
 * Function used to read the ADCIRC mesh into the class. Assumes the filename has not already
 * been specified. The mesh file will be read in after checking for some simple errors.
 * Error codes will be returned upon any error. Any return besides ADCMESH_NOERROR is a
 * fatal error
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::read(QString inputFile)
{
    //...Set the filename
    this->filename = inputFile;

    //...Check for a null string
    if(this->filename==QString())
    {
        this->errorCode = ADCMESH_NULLFILENAM;
        return this->errorCode;
    }

    //...Check for file exists
    QFile thisFile(this->filename);
    if(!thisFile.exists())
    {
        this->errorCode = ADCMESH_FILENOEXIST;
        return this->errorCode;
    }

    //...Assuming these two checks passed, we can call
    //   the main routine
    int ierr = this->readMesh();

    return ierr;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...The publicly exposed function to write an ADCIRC mesh
//-----------------------------------------------------------------------------------------//
/** \brief  Public function to allow user to trigger writing
 * of the ADCIRC mesh contained within this class
 *
 *  \author Zach Cobell
 *
 *  @param  outputFile [in] Name of the file to write
 *
 * Function used by the user to write the ADCIRC mesh currently contained within the class.
 * If the file already exists, it will be overwritten. Tread carefully.
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::write(QString outputFile)
{
    if(outputFile==QString())
    {
        this->errorCode = ADCMESH_NULLFILENAM;
        return this->errorCode;
    }

    //...Assuming the filename is valid, write the mesh
    //int ierr = this->writeMesh(outputFile);

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...The publicly exposed function to set the ignoreMeshnumbering variable
//-----------------------------------------------------------------------------------------//
/** \brief Public function to determine if non-sequential mesh numbering is a fatal error
 *
 * \author Zach Cobell
 *
 * @param  value [in] if true, mesh numbering is ignored.
 * If false, mesh numbering that is nonsequential is fatal
 *
 * Function used by the user to set if the code will determine that non-sequantial
 * mesh numbering is a fatal error. This applies to both the node portion of the file
 * as well as the element lists. This code can handle misnumbered meshes, however, ADCIRC
 * itself cannot.
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::setIgnoreMeshNumbering(bool value)
{
    this->ignoreMeshNumbering = value;
    this->errorCode = ADCMESH_NOERROR;
    return this->errorCode;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//
//
//   P R O T E C T E D
//             F U N C T I O N S
//
//
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
int adcirc_mesh::initializeErrors()
{
    //...Start by saying we haven't had any errors yet
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

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to read an ADCIRC mesh
//-----------------------------------------------------------------------------------------//
/** \brief This function is used internally to read an ADCIRC mesh
 *
 * \author Zach Cobell
 *
 * This function is used internally to read an ADCIRC mesh. The mesh will have its filename
 * specified in the adcirc_mesh::filename variable.
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readMesh()
{

    QFile       meshFile(this->filename);
    QString     meshStringData,tempString;
    QStringList meshFileList,tempList;
    QByteArray  meshByteData;
    int         i,ierr,filePosition;
    bool        err;

    if(!meshFile.open(QIODevice::ReadOnly))
    {
        this->errorCode = ADCMESH_FILEOPENERR;
        return this->errorCode;
    }

    //...Read the entire mesh file at once
    meshByteData   = meshFile.readAll();
    meshStringData = QString(meshByteData);
    meshFileList   = meshStringData.split("\n");
    meshByteData   = QByteArray();
    meshStringData = QString();
    meshFile.close();

    //...Grab the header
    this->title = meshFileList.value(0);

    //...Grab the number of elements,nodes
    tempString = meshFileList.value(1);
    tempList   = tempString.simplified().split(" ");
    tempString = tempList.value(1);
    this->numNodes = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_HEADER;
        return this->errorCode;
    }
    tempString = tempList.value(0);
    this->numElements = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_HEADER;
        return this->errorCode;
    }

    //...Allocate the nodes
    this->allocateNodes();

    //...Allocate the elements
    this->allocateElements();

    //...Loop over the nodes
    for(i=0;i<this->numNodes;i++)
    {
        ierr = this->readNode(meshFileList.value(i+2),i,this->nodes[i]);
        if(ierr!=ADCMESH_NOERROR)
        {
            this->errorCode = ierr;
            return this->errorCode;
        }

        //...Save the mapping. This is used to prevent
        //   issues with meshes that are not numbered sequentially
        this->nodeMapping[this->nodes[i]->id] = i;
    }

    //...Loop over the elements
    for(i=0;i<this->numElements;i++)
    {
        ierr = this->readElement(meshFileList.value(this->numNodes+2+i),i,this->elements[i]);
        if(ierr!=ADCMESH_NOERROR)
        {
            this->errorCode = ierr;
            return this->errorCode;
        }

        //...Save the mapping. This is used to prevent
        //   issues with meshes that are not numbered sequentially
        this->elementMapping[this->elements[i]->id] = i;
    }

    //...Read the open boundary
    filePosition = this->numNodes + this->numElements + 2;
    ierr = this->readOpenBoundaries(filePosition,meshFileList);
    if(ierr!=ADCMESH_NOERROR)
        return ierr;

    //...Read the land boundaries
    this->readLandBoundaries(filePosition,meshFileList);
    if(ierr!=ADCMESH_NOERROR)
        return ierr;

    //...The mesh read is now complete. We're done.

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Allocates an array of pointers in the ADCIRC mesh
//-----------------------------------------------------------------------------------------//
/** \brief Creates a number of adcirc_node variables on the heap
 *
 * \author Zach Cobell
 *
 * This function creates a set of ADCIRC nodes on the heap. All nodes that area
 * created are done so with a QObject reference to enable automatic memory management
 * to avoid memory leaks
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::allocateNodes()
{
    this->nodes.resize(this->numNodes);
    for(int i=0;i<this->numNodes;i++)
        this->nodes[i] = new adcirc_node(this);
    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Allocates an array of pointers in the ADCIRC mesh
//-----------------------------------------------------------------------------------------//
/** \brief Creates a number of adcirc_element variables on the heap
 *
 * \author Zach Cobell
 *
 * This function creates a set of ADCIRC elements on the heap. All elements that area
 * created are done so with a QObject reference to enable automatic memory management
 * to avoid memory leaks
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::allocateElements()
{
    this->elements.resize(this->numElements);
    for(int i=0;i<this->numElements;i++)
        this->elements[i] = new adcirc_element(this);
    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Parses a line from the ADCIRC mesh that contains a node into
//   its positional information. Note that if the mesh needs re-numbering,
//   you will need to set ignoreMeshNumbering to true
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to parse the string from an ADCIRC mesh file containing nodal information
 *
 * \author Zach Cobell
 *
 * @param line  [in]    The QString with the information read from the file to be parsed into an ADCIRC node
 * @param index [in]    An integer for the current node position in the ADCIRC file
 * @param *node [inout] The pointer to the adcirc_node that should be created with this information
 *
 * This function can parse a single line from an ADCIRC mesh file to break it into the nodal information
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readNode(QString line, int index, adcirc_node *node)
{
    QStringList tempList;
    QString     tempString;
    int         tempInt;
    double      tempDouble;
    bool        err;

    tempList   = line.simplified().split(" ");
    tempString = tempList.value(0);
    tempInt    = tempString.toInt(&err);
    if(!err)
        return ADCMESH_MESHREAD_NODERR;

    if(tempInt!=index+1)
    {
        if(this->ignoreMeshNumbering)
            this->meshNeedsNumbering = true;
        else
            return ADCMESH_MESHREAD_ELENUM;
    }
    node->id = tempInt;

    tempString = tempList.value(1);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_NODERR;
    node->x = tempDouble;
    tempString = tempList.value(2);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_NODERR;
    node->y = tempDouble;
    tempString = tempList.value(3);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return ADCMESH_MESHREAD_NODERR;
    node->z = tempDouble;

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Parses a line from the ADCIRC mesh that contains an element into
//   its connectivity information. Note that if the mesh needs re-numbering,
//   you will need to set ignoreMeshNumbering to true;
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to parse the string from an ADCIRC mesh file containing
 *  elemental connectivity information
 *
 * \author Zach Cobell
 *
 * @param line     [in]    The QString with the information read from the file to be parsed into an ADCIRC element
 * @param index    [in]    An integer for the current element position in the ADCIRC file
 * @param *element [inout] The pointer to the adcirc_element that should be created with this information
 *
 * This function parses the string from the element section of the ADCIRC file
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readElement(QString line, int index, adcirc_element *element)
{
    QStringList tempList;
    QString     tempString;
    int         tempInt;
    bool        err;

    tempList   = line.simplified().split(" ");
    tempString = tempList.value(0);
    tempInt    = tempString.toInt(&err);
    if(!err)
        return ADCMESH_MESHREAD_ELEMER;
    if(tempInt!=index+1)
    {
        if(this->ignoreMeshNumbering)
            this->meshNeedsNumbering = true;
        else
            return ADCMESH_MESHREAD_ELENUM;
    }
    element->id = tempInt;

    tempString = tempList.value(2);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return ADCMESH_MESHREAD_ELEMER;
    element->connections[0] = this->nodes[this->nodeMapping[tempInt]];

    tempString = tempList.value(3);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return ADCMESH_MESHREAD_ELEMER;
    element->connections[1] = this->nodes[this->nodeMapping[tempInt]];

    tempString = tempList.value(4);
    tempInt    = tempString.simplified().toInt(&err);
    if(!err)
        return ADCMESH_MESHREAD_ELEMER;
    element->connections[2] = this->nodes[this->nodeMapping[tempInt]];

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to read the open boundary segmenets of the adcirc mesh file
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the entire set of open boundary conditions
 *
 * \author Zach Cobell
 *
 * @param position [inout] The current file position. Returned as the new file position when the open boundary read is complete
 * @param fileData [in]    Reference to the data read from the ADCIRC mesh file
 */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readOpenBoundaries(int &position, QStringList &fileData)
{
    QString tempString;
    int i,j,tempInt;
    bool err;

    //...Read the header
    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->numOpenBoundaries = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }

    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->totNumOpenBoundaryNodes = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }

    //...Allocate the boundary array
    this->openBC.resize(this->numOpenBoundaries);

    //...Read the boundaries
    for(i=0;i<this->numOpenBoundaries;i++)
    {
        //...Read the number of nodes in the boundary
        tempString = fileData[position];
        position = position + 1;
        tempString = tempString.simplified().split(" ").value(0);

        //...Create a new adcirc_boundary object on the heap
        this->openBC[i] = new adcirc_boundary(this);

        //...Set a default code so we know it is an open boundary
        //   if for some reason we don't realize it
        this->openBC[i]->code = -9999;

        //...Get the size of this open boundary
        this->openBC[i]->numNodes = tempString.toInt(&err);
        if(!err)
        {
            this->errorCode = ADCMESH_MESHREAD_BNDERR;
            return this->errorCode;
        }

        //...Resize the vector
        this->openBC[i]->n1.resize(this->openBC[i]->numNodes);

        //...Loop over the length of the boundary
        for(j=0;j<this->openBC[i]->numNodes;j++)
        {
            tempString = fileData[position];
            position = position + 1;
            tempString = tempString.simplified().split(" ").value(0);
            tempInt = tempString.toInt(&err);
            if(!err)
            {
                this->errorCode = ADCMESH_MESHREAD_BNDERR;
                return this->errorCode;
            }

            //...Use the mapping instead of a direct lookup to avoid issues
            //   with misnumbered meshes
            this->openBC[i]->n1[j] = this->nodes[this->nodeMapping[tempInt]];
        }
    }

    return ADCMESH_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to read the land boundary segments from the ADCIRC mesh file
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the entire set of land boundary conditions
 *
 * \author Zach Cobell
 *
 * @param position [inout] The current file positoin. Returned as the new file position when the land boundary read is complete
 * @param fileData [in]    Reference to the data read from the ADCIRC mesh file
 */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readLandBoundaries(int &position, QStringList &fileData)
{
    QString tempString,tempString2;
    int ierr,i,j;
    bool err;

    //...Read the header
    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->numLandBoundaries = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }

    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->totNumLandBoundaryNodes = tempString.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }

    //...Allocate the boundary array
    this->landBC.resize(this->numLandBoundaries);

    //...Read the boundaries
    for(i=0;i<this->numLandBoundaries;i++)
    {
        //...Read the number of nodes in the boundary
        tempString = fileData[position];
        position   = position + 1;

        //...Create a new adcirc_boundary object on the heap
        this->landBC[i] = new adcirc_boundary(this);

        //...Set a default code so we know it is an open boundary
        //   if for some reason we don't realize it
        tempString2 = tempString.simplified().split(" ").value(1);
        this->landBC[i]->code = tempString2.toInt(&err);
        if(!err)
        {
            this->errorCode = ADCMESH_MESHREAD_BNDERR;
            return this->errorCode;
        }

        //...Get the size of this land boundary
        tempString2 = tempString.simplified().split(" ").value(0);
        this->landBC[i]->numNodes = tempString2.toInt(&err);
        if(!err)
        {
            this->errorCode = ADCMESH_MESHREAD_BNDERR;
            return this->errorCode;
        }

        //...Read the boundary string depending on its type
        if(this->landBC[i]->code  == 0   ||
            this->landBC[i]->code == 1   ||
            this->landBC[i]->code == 2   ||
            this->landBC[i]->code == 10  ||
            this->landBC[i]->code == 11  ||
            this->landBC[i]->code == 12  ||
            this->landBC[i]->code == 20  ||
            this->landBC[i]->code == 21  ||
            this->landBC[i]->code == 22  ||
            this->landBC[i]->code == 30  ||
            this->landBC[i]->code == 52  ||
            this->landBC[i]->code == 102 ||
            this->landBC[i]->code == 112 ||
            this->landBC[i]->code == 122 )
        {
            this->landBC[i]->n1.resize(this->landBC[i]->numNodes);
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {
                tempString = fileData[position];
                position = position + 1;
                ierr = this->readLandBoundarySingleNode(tempString,this->landBC[i],j);
                if(ierr!=ADCMESH_NOERROR)
                {
                    this->errorCode = ierr;
                    return this->errorCode;
                }
            }
        }
        else if(this->landBC[i]->code == 3  ||
                this->landBC[i]->code == 13 ||
                this->landBC[i]->code == 23 )
        {
            this->landBC[i]->n1.resize(this->landBC[i]->numNodes);
            this->landBC[i]->crest.resize(this->landBC[i]->numNodes);
            this->landBC[i]->supercritical.resize(this->landBC[i]->numNodes);
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {

                tempString = fileData[position];
                position   = position + 1;
                ierr = this->readLandBoundaryOneSidedWeir(tempString,this->landBC[i],j);
                if(ierr!=ADCMESH_NOERROR)
                {
                    this->errorCode = ierr;
                    return this->errorCode;
                }
            }
        }
        else if(this->landBC[i]->code == 4  ||
                this->landBC[i]->code == 24 )
        {
            this->landBC[i]->n1.resize(this->landBC[i]->numNodes);
            this->landBC[i]->n2.resize(this->landBC[i]->numNodes);
            this->landBC[i]->crest.resize(this->landBC[i]->numNodes);
            this->landBC[i]->supercritical.resize(this->landBC[i]->numNodes);
            this->landBC[i]->subcritical.resize(this->landBC[i]->numNodes);
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {
                tempString = fileData[position];
                position   = position + 1;
                ierr = this->readLandBoundaryTwoSidedWeir(tempString,this->landBC[i],j);
            }
        }
        else if(this->landBC[i]->code == 5  ||
                this->landBC[i]->code == 25 )
        {
            this->landBC[i]->n1.resize(this->landBC[i]->numNodes);
            this->landBC[i]->n2.resize(this->landBC[i]->numNodes);
            this->landBC[i]->crest.resize(this->landBC[i]->numNodes);
            this->landBC[i]->supercritical.resize(this->landBC[i]->numNodes);
            this->landBC[i]->subcritical.resize(this->landBC[i]->numNodes);
            this->landBC[i]->pipeCoef.resize(this->landBC[i]->numNodes);
            this->landBC[i]->pipeDiam.resize(this->landBC[i]->numNodes);
            this->landBC[i]->pipeHeight.resize(this->landBC[i]->numNodes);
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {
                tempString = fileData[position];
                position = position + 1;
                this->readLandBoundaryCrossBarrierPipe(tempString,this->landBC[i],j);
            }
        }
        else
        {
            this->landBC[i]->n1.resize(this->landBC[i]->numNodes);
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {
                tempString = fileData[position];
                position = position + 1;
                this->readLandBoundarySingleNode(tempString,this->landBC[i],j);
            }
        }
    }

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse the data from an ADCIRC mesh file for a single node boundary
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the single node data from a land boundary string (single node types)
 *
 * \author Zach Cobell
 *
 * @param data     [in]    The data found in the adcirc file to be parsed
 * @param boundary [inout] A pointer to the boundary we are working on
 * @param index    [in]    Location in the boundary vector for the data passed to the function
 */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readLandBoundarySingleNode(QString data, adcirc_boundary *boundary, int index)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);

    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }

    boundary->n1[index] = this->nodes[this->nodeMapping[tempInt]];

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a one-sided weir boundary (type 3,13)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the one sided weir data from a land boundary string (type 3,13)
 *
 * \author Zach Cobell
 *
 * @param data     [in]    The data found in the adcirc file to be parsed
 * @param boundary [inout] A pointer to the boundary we are working on
 * @param index    [in]    Location in the boundary vector for the data passed to the function */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readLandBoundaryOneSidedWeir(QString data, adcirc_boundary *boundary, int index)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    double  tempDouble;

    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->n1[index] = this->nodes[this->nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->supercritical[index] = tempDouble;

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a two-sided weir (type 4,24)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the one sided two sided weir information from a
 * land boundary string (type 4,24)
 *
 * \author Zach Cobell
 *
 * @param data     [in]    The data found in the adcirc file to be parsed
 * @param boundary [inout] A pointer to the boundary we are working on
 * @param index    [in]    Location in the boundary vector for the data passed to the function */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readLandBoundaryTwoSidedWeir(QString data, adcirc_boundary *boundary, int index)
{

    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    double  tempDouble;

    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->n1[index] = this->nodes[this->nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempInt     = tempString2.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->n2[index] = this->nodes[this->nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(3);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->subcritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(4);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->supercritical[index] = tempDouble;

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to parse a two-sided weir with cross-barrier pipes (type 5,25)
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the so-called "leaky weir", or weir with cross
 *  barrier pipes from a land boundary string (type 5,25)
 *
 * \author Zach Cobell
 *
 * @param data     [in]    The data found in the adcirc file to be parsed
 * @param boundary [inout] A pointer to the boundary we are working on
 * @param index    [in]    Location in the boundary vector for the data passed to the function */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readLandBoundaryCrossBarrierPipe(QString data, adcirc_boundary *boundary, int index)
{
    bool    err;
    QString tempString2 = data.simplified().split(" ").value(0);
    int     tempInt     = tempString2.toInt(&err);
    double  tempDouble;

    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->n1[index] = this->nodes[this->nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(1);
    tempInt     = tempString2.toInt(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->n2[index] = this->nodes[this->nodeMapping[tempInt]];

    tempString2 = data.simplified().split(" ").value(2);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->crest[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(3);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->subcritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(4);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->supercritical[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(5);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->pipeHeight[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(6);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->pipeCoef[index] = tempDouble;

    tempString2 = data.simplified().split(" ").value(7);
    tempDouble  = tempString2.toDouble(&err);
    if(!err)
    {
        this->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->errorCode;
    }
    boundary->pipeDiam[index] = tempDouble;

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//
