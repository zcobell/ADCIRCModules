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
    this->error = new adcirc_errors(this);

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
        this->error->errorCode = ADCMESH_NULLFILENAM;
        return this->error->errorCode;
    }

    //...Check for file exists
    QFile thisFile(this->filename);
    if(!thisFile.exists())
    {
        this->error->errorCode = ADCMESH_FILENOEXIST;
        return this->error->errorCode;
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
        this->error->errorCode = ADCMESH_NULLFILENAM;
        return this->error->errorCode;
    }

    //...Check for file exists
    QFile thisFile(this->filename);
    if(!thisFile.exists())
    {
        this->error->errorCode = ADCMESH_FILENOEXIST;
        return this->error->errorCode;
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
        this->error->errorCode = ADCMESH_NULLFILENAM;
        return this->error->errorCode;
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
    this->error->errorCode = ADCMESH_NOERROR;
    return this->error->errorCode;
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
        this->error->errorCode = ADCMESH_FILEOPENERR;
        return this->error->errorCode;
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
        this->error->errorCode = ADCMESH_MESHREAD_HEADER;
        return this->error->errorCode;
    }
    tempString = tempList.value(0);
    this->numElements = tempString.toInt(&err);
    if(!err)
    {
        this->error->errorCode = ADCMESH_MESHREAD_HEADER;
        return this->error->errorCode;
    }

    //...Allocate the nodes
    this->allocateNodes();

    //...Allocate the elements
    this->allocateElements();

    //...Loop over the nodes
    for(i=0;i<this->numNodes;i++)
    {

        ierr = this->nodes[i]->readNode(meshFileList.value(i+2));

        if(ierr!=ADCMESH_NOERROR)
        {
            this->error->errorCode = ierr;
            return this->error->errorCode;
        }

        //...Check that the numbering is acceptable
        if(this->nodes[i]->id != i+1)
        {
            if(this->ignoreMeshNumbering)
                this->meshNeedsNumbering = true;
            else
            {
                this->error->errorCode = ADCMESH_MESHREAD_NODNUM;
                return this->error->errorCode;
            }
        }

        //...Save the mapping. This is used to prevent
        //   issues with meshes that are not numbered sequentially
        this->nodeMapping[this->nodes[i]->id] = i;
    }

    //...Loop over the elements
    for(i=0;i<this->numElements;i++)
    {
        ierr = this->elements[i]->readElement(meshFileList.value(this->numNodes+2+i),this->nodes,this->nodeMapping);
        if(ierr!=ADCMESH_NOERROR)
        {
            this->error->errorCode = ierr;
            return this->error->errorCode;
        }

        //...Check that the numbering is acceptable
        if(this->elements[i]->id != i+1)
        {
            if(this->ignoreMeshNumbering)
                this->meshNeedsNumbering = true;
            else
            {
                this->error->errorCode = ADCMESH_MESHREAD_ELENUM;
                return this->error->errorCode;
            }
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
    int i,j,ierr;
    int boundaryCode,boundarySize;
    bool err;

    //...Read the header
    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->numOpenBoundaries = tempString.toInt(&err);
    if(!err)
    {
        this->error->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->error->errorCode;
    }

    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->totNumOpenBoundaryNodes = tempString.toInt(&err);
    if(!err)
    {
        this->error->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->error->errorCode;
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

        //...Set a default code so we know it is an open boundary
        //   if for some reason we don't realize it
        boundaryCode = -9999;

        //...Get the size of this open boundary
        boundarySize = tempString.toInt(&err);
        if(!err)
        {
            this->error->errorCode = ADCMESH_MESHREAD_BNDERR;
            return this->error->errorCode;
        }

        //...Create a new adcirc_boundary object on the heap
        this->openBC[i] = new adcirc_boundary(boundaryCode,boundarySize,this);

        //...Loop over the length of the boundary
        for(j=0;j<this->openBC[i]->numNodes;j++)
        {
            tempString = fileData[position];
            position = position + 1;
            ierr = this->openBC[i]->readLandBoundaryString(tempString,j,this->nodes,this->nodeMapping);
            if(ierr!=ADCMESH_NOERROR)
            {
                this->error->errorCode = ierr;
                return this->error->errorCode;
            }
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
 * @param position [inout] The current file position. Returned as the new file position when the land boundary read is complete
 * @param fileData [in]    Reference to the data read from the ADCIRC mesh file
 */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readLandBoundaries(int &position, QStringList &fileData)
{
    QString tempString,tempString2;
    int ierr,i,j;
    int boundaryCode,boundarySize;
    bool err;

    //...Read the header
    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->numLandBoundaries = tempString.toInt(&err);
    if(!err)
    {
        this->error->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->error->errorCode;
    }

    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->totNumLandBoundaryNodes = tempString.toInt(&err);
    if(!err)
    {
        this->error->errorCode = ADCMESH_MESHREAD_BNDERR;
        return this->error->errorCode;
    }

    //...Allocate the boundary array
    this->landBC.resize(this->numLandBoundaries);

    //...Read the boundaries
    for(i=0;i<this->numLandBoundaries;i++)
    {
        //...Read the number of nodes in the boundary
        tempString = fileData[position];
        position   = position + 1;

        //...Set a default code so we know it is an open boundary
        //   if for some reason we don't realize it
        tempString2 = tempString.simplified().split(" ").value(1);
        boundaryCode = tempString2.toInt(&err);
        if(!err)
        {
            this->error->errorCode = ADCMESH_MESHREAD_BNDERR;
            return this->error->errorCode;
        }

        //...Get the size of this land boundary
        tempString2 = tempString.simplified().split(" ").value(0);
        boundarySize = tempString2.toInt(&err);
        if(!err)
        {
            this->error->errorCode = ADCMESH_MESHREAD_BNDERR;
            return this->error->errorCode;
        }

        //...Create a new adcirc_boundary object on the heap
        this->landBC[i] = new adcirc_boundary(boundaryCode,boundarySize,this);

        //...Read the boundary string depending on its type
        for(j=0;j<this->landBC[i]->numNodes;j++)
        {
            tempString = fileData[position];
            position = position + 1;
            ierr = this->landBC[i]->readLandBoundaryString(tempString,j,this->nodes,this->nodeMapping);
            if(ierr!=ADCMESH_NOERROR)
            {
                this->error->errorCode = ierr;
                return this->error->errorCode;
            }
        }
    }

    return ADCMESH_NOERROR;
}
//-----------------------------------------------------------------------------------------//

