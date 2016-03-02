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

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for the ADCIRC mesh class
 *
 * \author Zach Cobell
 *
 * @param *parent [in] reference to QObject. Enables automatic memory management to avoid memory leaks
 *
 * Constructs an adcirc_mesh object, takes QObject reference as input
 *
 **/
//-----------------------------------------------------------------------------------------//
adcirc_mesh::adcirc_mesh(QObject *parent) : QObject(parent)
{
    //...Initialize the errors so we can
    //   describe what went wrong to the
    //   user later    
    this->error = new adcirc_errors(this);

    //...By default, we will assume that the mesh numbering should be sequential
    this->ignoreMeshNumbering = false;
    this->meshNeedsNumbering  = false;

    //...Assume this is a geographic coordinate system by default. After
    //   the mesh is read, this will be checked to be sure
    this->isGCS = true;

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
 * will be returned upon any error. Any return besides ERROR_NOERROR is a fatal
 * error
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::read()
{
    //...Check for a null string
    if(this->filename==QString())
    {
        this->error->setError(ERROR_NULLFILENAM);
        return this->error->getError();
    }

    //...Check for file exists
    QFile thisFile(this->filename);
    if(!thisFile.exists())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
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
 * Error codes will be returned upon any error. Any return besides ERROR_NOERROR is a
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
        this->error->setError(ERROR_NULLFILENAM);
        return this->error->getError();
    }

    //...Check for file exists
    QFile thisFile(this->filename);
    if(!thisFile.exists())
    {
        this->error->setError(ERROR_FILENOEXIST);
        return this->error->getError();
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
        this->error->setError(ERROR_NULLFILENAM);
        return this->error->getError();
    }

    //...Assuming the filename is valid, write the mesh
    int ierr = this->writeMesh(outputFile);

    return ierr;
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
    this->error->setError(ERROR_NOERROR);
    return this->error->getError();
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
        this->error->setError(ERROR_FILEOPENERR);
        return this->error->getError();
    }

    //...Read the entire mesh file at once
    meshByteData   = meshFile.readAll();
    meshStringData = QString(meshByteData);
    meshFileList   = meshStringData.split("\n");
    meshByteData   = QByteArray();
    meshStringData = QString();
    meshFile.close();

    //...Grab the header
    tempString = meshFileList.value(0);
    this->title = tempString.simplified();

    //...Grab the number of elements,nodes
    tempString = meshFileList.value(1);
    tempList   = tempString.simplified().split(" ");
    tempString = tempList.value(1);
    this->numNodes = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_MESHREAD_HEADER);
        return this->error->getError();
    }
    tempString = tempList.value(0);
    this->numElements = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_MESHREAD_HEADER);
        return this->error->getError();
    }

    //...Do a quick check on the length of the file
    if(meshFileList.length()<this->numNodes+this->numElements+2)
    {
        this->error->setError(ERROR_MESHREAD_UNEXPECTEDEND);
        return this->error->getError();
    }

    //...Allocate the nodes
    this->allocateNodes();

    //...Allocate the elements
    this->allocateElements();

    //...Loop over the nodes
    for(i=0;i<this->numNodes;i++)
    {

        ierr = this->nodes[i]->fromString(meshFileList.value(i+2));

        if(ierr!=ERROR_NOERROR)
        {
            this->error->setError(ierr);
            return this->error->getError();
        }

        //...Check that the numbering is acceptable
        if(this->nodes[i]->id != i+1)
        {
            if(this->ignoreMeshNumbering)
                this->meshNeedsNumbering = true;
            else
            {
                this->error->setError(ERROR_MESHREAD_NODNUM);
                return this->error->getError();
            }
        }

        //...Save the mapping. This is used to prevent
        //   issues with meshes that are not numbered sequentially
        this->nodeMapping[this->nodes[i]->id] = i;
    }

    //...Loop over the elements
    for(i=0;i<this->numElements;i++)
    {
        ierr = this->elements[i]->fromString(meshFileList.value(this->numNodes+2+i),this->nodes,this->nodeMapping);
        if(ierr!=ERROR_NOERROR)
        {
            this->error->setError(ierr);
            return this->error->getError();
        }

        //...Check that the numbering is acceptable
        if(this->elements[i]->id != i+1)
        {
            if(this->ignoreMeshNumbering)
                this->meshNeedsNumbering = true;
            else
            {
                this->error->setError(ERROR_MESHREAD_ELENUM);
                return this->error->getError();
            }
        }

        //...Save the mapping. This is used to prevent
        //   issues with meshes that are not numbered sequentially
        this->elementMapping[this->elements[i]->id] = i;
    }

    //...Read the open boundary
    filePosition = this->numNodes + this->numElements + 2;
    ierr = this->readOpenBoundaries(filePosition,meshFileList);
    if(ierr!=ERROR_NOERROR)
        return ierr;

    //...Read the land boundaries
    this->readLandBoundaries(filePosition,meshFileList);
    if(ierr!=ERROR_NOERROR)
        return ierr;

    //...Determine how we are going to write back the coordinates
    this->senseCoordinateSystem();

    //...The mesh read is now complete. We're done.

    return ERROR_NOERROR;
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
    return ERROR_NOERROR;
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
    return ERROR_NOERROR;
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
 *
 * Protected function to read the entire set of open boundary conditions
 */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readOpenBoundaries(int &position, QStringList &fileData)
{
    QString tempString;
    int i,j,ierr;
    int boundaryCode,boundarySize;
    bool err;

    if(position>fileData.length()-1)
        return ERROR_MESHREAD_UNEXPECTEDEND;

    //...Read the header
    tempString = fileData[position];

    position = position + 1;

    tempString = tempString.simplified().split(" ").value(0);
    this->numOpenBoundaries = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_MESHREAD_BNDERR);
        return this->error->getError();
    }

    if(position>fileData.length()-1)
        return ERROR_MESHREAD_UNEXPECTEDEND;

    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->totNumOpenBoundaryNodes = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_MESHREAD_BNDERR);
        return this->error->getError();
    }

    //...Allocate the boundary array
    this->openBC.resize(this->numOpenBoundaries);

    //...Read the boundaries
    for(i=0;i<this->numOpenBoundaries;i++)
    {
        if(position>fileData.length()-1)
        {
            this->error->setError(ERROR_MESHREAD_UNEXPECTEDEND);
            return this->error->getError();
        }

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
            this->error->setError(ERROR_MESHREAD_BNDERR);
            return this->error->getError();
        }

        //...Create a new adcirc_boundary object on the heap
        this->openBC[i] = new adcirc_boundary(boundaryCode,boundarySize,this);

        //...Loop over the length of the boundary
        for(j=0;j<this->openBC[i]->numNodes;j++)
        {
            if(position>fileData.length()-1)
            {
                this->error->setError(ERROR_MESHREAD_UNEXPECTEDEND);
                return this->error->getError();
            }

            tempString = fileData[position];
            position = position + 1;
            ierr = this->openBC[i]->fromString(tempString,j,this->nodes,this->nodeMapping);
            if(ierr!=ERROR_NOERROR)
            {
                this->error->setError(ierr);
                return this->error->getError();
            }
        }
    }

    return ERROR_NOERROR;

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to read the land boundary segments from the ADCIRC mesh file
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to read the entire set of land boundary conditions
 *
 * \author Zach Cobell
 *
 * @param position [inout] The current file position. Returned as the new file position
 *                         when the land boundary read is complete
 * @param fileData [in]    Reference to the data read from the ADCIRC mesh file
 *
 * Protected function to read the entire set of land boundary conditions
 */
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readLandBoundaries(int &position, QStringList &fileData)
{
    QString tempString,tempString2;
    int ierr,i,j;
    int boundaryCode,boundarySize;
    bool err;

    if(position>fileData.length()-1)
    {
        this->error->setError(ERROR_MESHREAD_UNEXPECTEDEND);
        return this->error->getError();
    }

    //...Read the header
    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->numLandBoundaries = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_MESHREAD_BNDERR);
        return this->error->getError();
    }

    if(position>fileData.length()-1)
    {
        this->error->setError(ERROR_MESHREAD_UNEXPECTEDEND);
        return this->error->getError();
    }

    tempString = fileData[position];
    position = position + 1;
    tempString = tempString.simplified().split(" ").value(0);
    this->totNumLandBoundaryNodes = tempString.toInt(&err);
    if(!err)
    {
        this->error->setError(ERROR_MESHREAD_BNDERR);
        return this->error->getError();
    }

    //...Allocate the boundary array
    this->landBC.resize(this->numLandBoundaries);

    //...Read the boundaries
    for(i=0;i<this->numLandBoundaries;i++)
    {
        if(position>fileData.length()-1)
        {
            this->error->setError(ERROR_MESHREAD_UNEXPECTEDEND);
            return this->error->getError();
        }

        //...Read the number of nodes in the boundary
        tempString = fileData[position];
        position   = position + 1;

        //...Set a default code so we know it is an open boundary
        //   if for some reason we don't realize it
        tempString2 = tempString.simplified().split(" ").value(1);
        boundaryCode = tempString2.toInt(&err);
        if(!err)
        {
            this->error->setError(ERROR_MESHREAD_BNDERR);
            return this->error->getError();
        }

        //...Get the size of this land boundary
        tempString2 = tempString.simplified().split(" ").value(0);
        boundarySize = tempString2.toInt(&err);
        if(!err)
        {
            this->error->setError(ERROR_MESHREAD_BNDERR);
            return this->error->getError();
        }

        //...Create a new adcirc_boundary object on the heap
        this->landBC[i] = new adcirc_boundary(boundaryCode,boundarySize,this);

        //...Read the boundary string depending on its type
        for(j=0;j<this->landBC[i]->numNodes;j++)
        {
            if(position>fileData.length()-1)
            {
                this->error->setError(ERROR_MESHREAD_UNEXPECTEDEND);
                return this->error->getError();
            }

            tempString = fileData[position];
            position = position + 1;
            ierr = this->landBC[i]->fromString(tempString,j,this->nodes,this->nodeMapping);
            if(ierr!=ERROR_NOERROR)
            {
                this->error->setError(ierr);
                return this->error->getError();
            }
        }
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to write an ADCIRC mesh
//-----------------------------------------------------------------------------------------//
/** \brief This function is used internally to write an ADCIRC mesh
 *
 * \author Zach Cobell
 *
 * @param filename [in] Name of the output mesh file
 *
 * This function is used internally to write an ADCIRC mesh.
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::writeMesh(QString filename)
{
    int i,j;
    QString tempString;
    QStringList boundaryList;

    QFile outputFile(filename);

    //...Open the output file. Return with error
    //   if any issues occur
    if(!outputFile.open(QIODevice::WriteOnly))
    {
        this->error->setError(ERROR_FILEOPENERR);
        return this->error->getError();
    }

    //...Create a stream
    QTextStream out(&outputFile);

    //...Write the header
    out << this->title << "\n";
    out << tempString.sprintf("%11i  %11i",this->numElements,this->numNodes) << "\n";

    //...Write the mesh nodes
    for(i=0;i<this->numNodes;i++)
        out << this->nodes[i]->toString(this->isGCS) << "\n";

    //...Write the mesh elements
    for(i=0;i<this->numElements;i++)
        out << this->elements[i]->toString() << "\n";

    //...Write the open boundary header
    out << this->numOpenBoundaries << "\n";
    out << this->totNumOpenBoundaryNodes << "\n";

    //...Write the open boundaries
    for(i=0;i<this->numOpenBoundaries;i++)
    {
        boundaryList = this->openBC[i]->toStringList(true);
        for(j=0;j<boundaryList.length();j++)
            out << boundaryList.value(j) << "\n";
    }

    //...Write the land boundary header
    out << this->numLandBoundaries << "\n";
    out << this->totNumLandBoundaryNodes << "\n";

    //...Write the land boundaries
    for(i=0;i<this->numLandBoundaries;i++)
    {
        boundaryList = this->landBC[i]->toStringList(false);
        for(j=0;j<boundaryList.length();j++)
            out << boundaryList.value(j) << "\n";
    }

    //...Close the file
    outputFile.close();

    return ERROR_NOERROR;

}

//-----------------------------------------------------------------------------------------//
//...Function to attempt to sense the coordinate system type
//-----------------------------------------------------------------------------------------//
/** \brief This function is used internally sense the type of coordinate system that is used
 *
 * \author Zach Cobell
 *
 * This function is used internally sense the type of coordinate system that is used.
 * Ultimately, it is used to format the coordinates when writing an output file so that
 * precision is maintained. UTM or other simliar coordinate systems will have fewer decimal
 * places than what appears to be a geographic coordinate system
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::senseCoordinateSystem()
{
    int i;
    qreal avgX,avgY,m1,m2,mag,nn;

    avgX = 0.0;
    avgY = 0.0;

    nn = static_cast<qreal>(this->numNodes);

    for(i=0;i<this->numNodes;i++)
    {
        avgX = avgX + this->nodes[i]->x;
        avgY = avgY + this->nodes[i]->y;
    }

    avgX = qAbs(avgX / nn);
    avgY = qAbs(avgY / nn);

    m1 = qAbs(qLn(avgX)/qLn(10.0));
    m2 = qAbs(qLn(avgY)/qLn(10.0));

    mag = ( m1 + m2 ) / 2.0;

    if(mag>3.0)
        this->isGCS = false;
    else
        this->isGCS = true;

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to renumber an adcirc mesh sequentially
//-----------------------------------------------------------------------------------------//
/** \brief This function is used to renumber an adcirc_mesh sequantially
 *
 * \author Zach Cobell
 *
 * This function is used to renumber an adcirc_mesh (nodes, elements) so that it is valid.
 * Note that no attention is paid to minimizing bandwidth or providing optimal numbering.
 * This only ensures that the mesh is valid.
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::renumber()
{
    int i;

    //...Renumber the nodes
    for(i=0;i<this->numNodes;i++)
        this->nodes[i]->id = i+1;

    //...Renumber the elements
    for(i=0;i<this->numElements;i++)
        this->elements[i]->id = i+1;

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to check that the levee heights in the mesh are sane
//-----------------------------------------------------------------------------------------//
/** \brief This function is used to check if there are levee heights that need to be raised
 *
 * \author Zach Cobell
 *
 * @param minAbovePrevailingTopo [in] [optional] Elevation that a levee must be above the
 *                                               prevailing topographic elevation.
 *                                               Default = 0.2
 *
 * This function is used to check that an adcirc_mesh does not have levee heights that fall
 * below the topography, and thus causing a fatal error when running ADCIRC
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::checkLeveeHeights(double minAbovePrevailingTopo)
{
    int i,j;
    double z1,z2,c;

    for(i=0;i<this->numLandBoundaries;i++)
    {
        if(this->landBC[i]->code ==  4 || this->landBC[i]->code==24 ||
           this->landBC[i]->code == 25 || this->landBC[i]->code==25)
        {
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {
                z1 = this->landBC[i]->n1[j]->z;
                z2 = this->landBC[i]->n2[j]->z;
                c  = this->landBC[i]->crest[j];

                if(z1 < c-minAbovePrevailingTopo || z2 < c-minAbovePrevailingTopo)
                {
                    this->error->setError(ERROR_LEVEE_BELOWTOPO);
                    return this->error->getError();
                }
            }
        }
        else if(this->landBC[i]->code == 3 || this->landBC[i]->code == 13 ||
                this->landBC[i]->code == 23)
        {
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {
                z1 = this->landBC[i]->n1[j]->z;
                c  = this->landBC[i]->crest[j];
                if(z1 < c-minAbovePrevailingTopo)
                {
                    this->error->setError(ERROR_LEVEE_BELOWTOPO);
                    return this->error->getError();
                }
            }
        }
    }
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to raise levees that fall below prevailing topography
//-----------------------------------------------------------------------------------------//
/** \brief This function is used to raise levees that fall below prevailing topography
 *
 * \author Zach Cobell
 *
 * @param numLeveesRaised        [out]           Number of levees that needed to be raised
 * @param maximumAmountRaised    [out]           Maximum amount that a levee needed to be raised
 * @param minAbovePrevailingTopo [in] [optional] Elevation a levee must be above prevailing topography. Default = 0.2
 * @param minRaise               [in] [optional] Minimum amount a levee may be raised. Default = 0.01
 * @param diagnosticFile         [in] [optional] Diagnostic file to write that will show where levees have been raised
 *                                               Output will be formatted as a csv file. Default = null
 *
 * This function is used to check that an adcirc_mesh does not have levee heights that fall
 * below the topography, and thus causing a fatal error when running ADCIRC. A diagnostic file
 * may be optionally written that shows the locations where a levee was elevated. If the diagnosticFile
 * variable is null, the file will not be written.
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::raiseLeveeHeights(int &numLeveesRaised, double &maximumAmountRaised,
                                   double minAbovePrevailingTopo, double minRaise, QString diagnosticFile)
{
    QFile diag(diagnosticFile);
    QTextStream diagOut(&diag);
    int i,j;
    double zm,c;
    double x,y;
    double minAllowableHeight,raiseAmount;
    bool writeOutputFile = false;

    numLeveesRaised = 0;
    maximumAmountRaised = 0.0;

    //...If the user wants a diagnostic file, prep the output
    if(diagnosticFile!=QString())
    {
        writeOutputFile = true;

        //...Check if we can open the file successfully
        if(!diag.open(QIODevice::WriteOnly))
        {
            this->error->setError(ERROR_FILEOPENERR);
            return this->error->getError();
        }

        //...Create an output stream
        diagOut << "X,Y,PrevailingTopo,OriginalLeveeCrest,AmountRaised\n";
    }

    for(i=0;i<this->numLandBoundaries;i++)
    {
        //...Two sided weirs
        if(this->landBC[i]->code ==  4 || this->landBC[i]->code==24 ||
           this->landBC[i]->code == 25 || this->landBC[i]->code==25)
        {
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {

                //...Get the max on both sides of the weir
                zm = qMax(-this->landBC[i]->n1[j]->z,-this->landBC[i]->n2[j]->z);

                //...Get the weir elevation
                c  = this->landBC[i]->crest[j];

                //...Define the minimum allowable height
                minAllowableHeight = zm+minAbovePrevailingTopo;

                //...Check if the weir is below the minimum allowable height
                if(c < minAllowableHeight)
                {
                    //...Determine how much the levee needs to be raised
                    raiseAmount = minAllowableHeight-c;

                    //...Ensure it is larger than the minimum raise
                    if(raiseAmount<minRaise)
                        raiseAmount = minRaise;

                    //...Apply the increase to the levee
                    this->landBC[i]->crest[j] = this->landBC[i]->crest[j]+raiseAmount;

                    //...Count this
                    numLeveesRaised = numLeveesRaised + 1;

                    //...Check if this is the largest amount raised
                    if(raiseAmount>maximumAmountRaised)
                        maximumAmountRaised = raiseAmount;

                    //...Write the output file if necessary
                    if(writeOutputFile)
                    {
                        x = (this->landBC[i]->n1[j]->x+this->landBC[i]->n2[j]->x)/2.0;
                        y = (this->landBC[i]->n1[j]->y+this->landBC[i]->n2[j]->y)/2.0;
                        diagOut << x << "," << y << "," << zm << "," << c << "," << raiseAmount << "\n";
                    }
                }

            }
        }
        else if(this->landBC[i]->code == 3 || this->landBC[i]->code == 13 ||
                this->landBC[i]->code == 23)
        {
            for(j=0;j<this->landBC[i]->numNodes;j++)
            {

                //...Get the prevailing ground elevation
                zm = -this->landBC[i]->n1[j]->z;

                //...Get the weir elevation
                c  = this->landBC[i]->crest[j];

                //...Define the minimum allowable height
                minAllowableHeight = zm+minAbovePrevailingTopo;

                //...Check if the weir is below the minimum allowable height
                if(c < minAllowableHeight)
                {
                    //...Determine how much the levee needs to be raised
                    raiseAmount = minAllowableHeight-c;

                    //...Ensure it is larger than the minimum raise
                    if(raiseAmount<minRaise)
                        raiseAmount = minRaise;

                    //...Apply the increase to the levee
                    this->landBC[i]->crest[j] = this->landBC[i]->crest[j]+raiseAmount;

                    //...Count this
                    numLeveesRaised = numLeveesRaised + 1;

                    //...Check if this is the largest amount raised
                    if(raiseAmount>maximumAmountRaised)
                        maximumAmountRaised = raiseAmount;

                    //...Write the output file if necessary
                    if(writeOutputFile)
                    {
                        x = this->landBC[i]->n1[j]->x;
                        y = this->landBC[i]->n1[j]->y;
                        diagOut << x << "," << y << "," << zm << "," << c << "," << raiseAmount << "\n";
                    }
                }

            }
        }
    }

    if(writeOutputFile)
        diag.close();

    return ERROR_NOERROR;
}
