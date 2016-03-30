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
#include "netcdf.h"
#include "shapefil.h"

//-----------------------------------------------------------------------------------------//
// Initializer that creates its own error object
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::adcirc_mesh(QObject *parent) : QObject(parent)
 * \brief Constructor for the ADCIRC mesh class
 *
 * @param[in] *parent reference to QObject. Enables automatic memory management to avoid memory leaks
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
    this->error = new QADCModules_errors(this);

    //...By default, we will assume that the mesh numbering should be sequential
    this->ignoreMeshNumbering = false;
    this->meshNeedsNumbering  = false;

    //...Assume this is a geographic coordinate system by default. After
    //   the mesh is read, this will be checked to be sure
    this->isLatLon = true;

    //...Initialize the coordinate system
    this->coordinateSystem = new proj4(this);

    //...Set the default coordinate system to WGS84
    this->epsg = 4326;

    return;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Initializer that creates its own error object
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::adcirc_mesh(QObject *parent) : QObject(parent)
 * \brief Constructor for the ADCIRC mesh class
 *
 * @param[in] *error  reference to an QADCModules_errors object to use for errors in this class
 * @param[in] *parent reference to QObject. Enables automatic memory management to avoid memory leaks
 *
 * Constructs an adcirc_mesh object, takes QObject reference as input
 *
 **/
//-----------------------------------------------------------------------------------------//
adcirc_mesh::adcirc_mesh(QADCModules_errors *error, QObject *parent) : QObject(parent)
{

    //...Use the error pointer that was passed into this object
    this->error = error;

    //...By default, we will assume that the mesh numbering should be sequential
    this->ignoreMeshNumbering = false;
    this->meshNeedsNumbering  = false;

    //...Assume this is a geographic coordinate system by default. After
    //   the mesh is read, this will be checked to be sure
    this->isLatLon = true;

    //...Initialize the coordinate system
    this->coordinateSystem = new proj4(this);

    //...Set the default coordinate system to WGS84
    this->epsg = 4326;

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
/**
 * \fn adcirc_mesh::read()
 * \brief Read an ADCIRC mesh into the class
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
    this->error->setError(ierr);

    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Public function to read an ADCIRC mesh. Assumes filename specified in input
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::read(QString inputFile)
 * \brief  Read an ADCIRC mesh into the class and set the filename
 *
 *   @param[in] inputFile Specifies the mesh file to be read
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
    this->error->setError(ierr);

    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Public function to read an ADCIRC mesh from netCDF. Assumes filename already specified
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::readNetCDF()
 * \brief Read an ADCIRC mesh from a netCDF file into the class
 *
 * @param[in] inputFile Specifies the mesh file to be read
 *
 * Function used to read the ADCIRC mesh into the class. Assumes the filename has already
 * been specified. The mesh file will be read in after checking for some simple errors. Error codes
 * will be returned upon any error. Any return besides ERROR_NOERROR is a fatal
 * error
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readNetCDF(QString inputFile)
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
    int ierr = this->readMeshFromNetCDF();
    this->error->setError(ierr);

    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Public function to read an ADCIRC mesh from netCDF. Assumes filename has not been specified
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::readNetCDF()
 * \brief Read an ADCIRC mesh from a netCDF file into the class
 *
 * Function used to read the ADCIRC mesh into the class. Assumes the filename has already
 * been specified. The mesh file will be read in after checking for some simple errors. Error codes
 * will be returned upon any error. Any return besides ERROR_NOERROR is a fatal
 * error
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readNetCDF()
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
    int ierr = this->readMeshFromNetCDF();
    this->error->setError(ierr);

    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...The publicly exposed function to write an ADCIRC mesh
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::write(QString outputFile)
 *
 * \brief  Public function to allow user to trigger writing
 * of the ADCIRC mesh contained within this class
 *
 *  @param[in]  outputFile Name of the file to write
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
/**
 * \fn adcirc_mesh::setIgnoreMeshNumbering(bool value)
 * \brief Public function to determine if non-sequential mesh numbering is a fatal error
 *
 * @param[in] value If true, mesh numbering is ignored. If false, mesh numbering that is nonsequential is fatal
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
//...Function to renumber an adcirc mesh sequentially
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::renumber()
 * \brief This function is used to renumber an adcirc_mesh sequantially
 *
 * This function is used to renumber an adcirc_mesh (nodes, elements) so that it is valid.
 * Note that no attention is paid to minimizing bandwidth or providing optimal numbering.
 * This only ensures that the mesh is valid.
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::renumber()
{
    int i;

    this->nodeToPositionMapping.clear();
    this->elementToPositionMapping.clear();

    //...Renumber the nodes
    for(i=0;i<this->numNodes;i++)
    {
        this->nodes[i]->id = i+1;
        this->nodeToPositionMapping[this->nodes[i]->id] = i;
        this->nodeToIdMapping[i] = this->nodes[i]->id;
    }

    //...Renumber the elements
    for(i=0;i<this->numElements;i++)
    {
        this->elements[i]->id = i+1;
        this->elementToPositionMapping[this->elements[i]->id] = i;
        this->elementToIdMapping[i] = this->elements[i]->id;
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to check that the levee heights in the mesh are sane
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::checkLeveeHeights(double minAbovePrevailingTopo)
 * \brief This function is used to check if there are levee heights that need to be raised
 *
 * @param[in] minAbovePrevailingTopo [optional] Elevation that a levee must be above the
 *                                              prevailing topographic elevation.
 *                                              Default = 0.2
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
                z1 = this->landBC[i]->n1[j]->position.z();
                z2 = this->landBC[i]->n2[j]->position.z();
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
                z1 = this->landBC[i]->n1[j]->position.z();
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
/**
 * \fn adcirc_mesh::raiseLeveeHeights(int &numLeveesRaised, double &maximumAmountRaised, double minAbovePrevailingTopo, double minRaise, QString diagnosticFile)
 * \brief This function is used to raise levees that fall below prevailing topography
 *
 * @param[out] numLeveesRaised                   Number of levees that needed to be raised
 * @param[out] maximumAmountRaised               Maximum amount that a levee needed to be raised
 * @param[in]  minAbovePrevailingTopo [optional] Elevation a levee must be above prevailing topography. Default = 0.2
 * @param[in]  minRaise               [optional] Minimum amount a levee may be raised. Default = 0.01
 * @param[in]  diagnosticFile         [optional] Diagnostic file to write that will show where levees have been raised
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
                zm = qMax(-this->landBC[i]->n1[j]->position.z(),-this->landBC[i]->n2[j]->position.z());

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
                        x = (this->landBC[i]->n1[j]->position.x()+this->landBC[i]->n2[j]->position.x())/2.0;
                        y = (this->landBC[i]->n1[j]->position.y()+this->landBC[i]->n2[j]->position.y())/2.0;
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
                zm = -this->landBC[i]->n1[j]->position.z();

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
                        x = this->landBC[i]->n1[j]->position.x();
                        y = this->landBC[i]->n1[j]->position.y();
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
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that checks for nodes that are not connected to any elements
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::checkDisjointNodes(int &numDisjointNodes, QList<adcirc_node*> &disjointNodeList)
 * \brief This function checks for nodes that are not connected to any elements
 *
 * @param[out] numDisjointNodes Number of nodes that are not connected to any element
 * @param[out] disjointNodeList List of adcirc_node* that are considered disjoint
 *
 * This function checks for nodes that are not connected to any elements
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::checkDisjointNodes(int &numDisjointNodes, QList<adcirc_node*> &disjointNodeList)
{
    int i,j;
    numDisjointNodes = 0;

    //...Reset the boolean values
    for(i=0;i<this->numNodes;i++)
        this->nodes[i]->myBool = false;

    //...Loop over the elements and reset the bool
    for(i=0;i<this->numElements;i++)
        for(j=0;j<this->elements[i]->numConnections;j++)
            this->elements[i]->connections[j]->myBool = true;

    //...Check to see how many nodes are considered disjoint
    for(i=0;i<this->numNodes;i++)
    {
        if(!this->nodes[i]->myBool)
        {
            disjointNodeList.append(this->nodes[i]);
            numDisjointNodes = numDisjointNodes + 1;
        }
    }

    if(numDisjointNodes!=0)
    {
        this->error->setError(ERROR_DISJOINTNODES);
        return this->error->getError();
    }
    else
    {
        this->error->setError(ERROR_NOERROR);
        return this->error->getError();
    }
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that eliminates nodes that are not connected to any elements
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::eliminateDisjointNodes(int &numDisjointNodes)
 * \brief This function eliminates nodes that are not connected to any elements
 *
 * @param[out] numDisjointNodes Number of nodes that were not connected to any element
 *
 * This function eliminates nodes that are not connected to any elements
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::eliminateDisjointNodes(int &numDisjointNodes)
{
    int i,j;
    numDisjointNodes = 0;

    //...Reset the boolean values
    for(i=0;i<this->numNodes;i++)
        this->nodes[i]->myBool = false;

    //...Loop over the elements and reset the bool
    for(i=0;i<this->numElements;i++)
        for(j=0;j<this->elements[i]->numConnections;j++)
            this->elements[i]->connections[j]->myBool = true;

    //...Eliminate nodes that are considered disjoint
    for(i=this->numNodes-1;i>=0;i--)
    {
        if(!this->nodes[i]->myBool)
        {
            numDisjointNodes = numDisjointNodes + 1;
            this->nodes.remove(i);
            this->numNodes = this->numNodes - 1;
        }
    }


    //...If the nodes have been altered, renumber the mesh
    //   otherwise, just return
    if(numDisjointNodes!=0)
    {
        //...Renumber the mesh
        this->renumber();

        //...Rebuild the element table
        this->buildElementTable();

        this->error->setError(ERROR_DISJOINTNODES);
        return this->error->getError();
    }
    else
    {
        this->error->setError(ERROR_NOERROR);
        return this->error->getError();
    }

}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that sets the current projection
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::setProjection(int epsg)
 * \brief Function that sets the current projection
 *
 * @param[in] epsg EPSG number for the coordinate system
 *
 * Function that sets the current projection
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::setProjection(int epsg)
{
    this->epsg = epsg;
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that converts an adcirc_mesh into another coordinate system
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::project(int epsg)
 * \brief Function that projects an adcirc_mesh into another coordinate system
 *
 * @param[in] epsg EPSG number for the new coordinate system
 *
 * This function projects the coordinate system of an adcirc_mesh via the Proj4 library
 * using EPSG IDs
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::project(int epsg)
{

    QVector<QPointF> inPoint,outPoint;
    int i,ierr;

    inPoint.resize(this->numNodes);
    outPoint.resize(this->numNodes);

    for(i=0;i<this->numNodes;i++)
        inPoint[i] = this->nodes[i]->position.toPointF();

    ierr = this->coordinateSystem->transform(this->epsg,epsg,inPoint,outPoint,this->isLatLon);

    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    for(i=0;i<this->numNodes;i++)
    {
        this->nodes[i]->position.setX(outPoint[i].x());
        this->nodes[i]->position.setY(outPoint[i].y());
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
//...Function that checks for overlapping elements
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::checkOverlappingElements(int &numOverlappingElements, QList<adcirc_element*> &overlappingElementList)
 * \brief Function that checks for overlapping elements in an adcirc_mesh
 *
 * @param[out] numOverlappingElements Number of duplicate elements found
 * @param[out] overlappingElementList List of duplicate elements in the mesh
 *
 * Function that checks for overlapping elements in an adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::checkOverlappingElements(int &numOverlappingElements, QList<adcirc_element*> &overlappingElementList)
{

    int i,j,k,l;
    int n1,n2;
    int m1,m2;
    int icount;

    numOverlappingElements = 0;
    overlappingElementList.clear();

    if(this->meshNeedsNumbering)
    {
        this->error->setError(ERROR_MESHREAD_NODNUM);
        return this->error->getError();
    }

    if(this->node_table.size()!=this->numNodes)
        this->buildElementTable();

    for(i=0;i<this->numElements;i++)
    {
        for(j=0;j<this->elements[i]->numConnections;j++)
        {
            icount = 0;
            if(j<this->elements[i]->numConnections-1)
            {
                n1 = this->elements[i]->connections[j]->id;
                n2 = this->elements[i]->connections[j+1]->id;
            }
            else
            {
                n1 = this->elements[i]->connections[j]->id;
                n2 = this->elements[i]->connections[0]->id;
            }
            for(k=0;k<this->node_table[n1-1]->elementsAroundNode.length();k++)
            {
                m1 = this->node_table[n1-1]->elementsAroundNode.at(k)->id;
                for(l=0;l<this->node_table[n2-1]->elementsAroundNode.length();l++)
                {
                    m2 = this->node_table[n2-1]->elementsAroundNode.at(l)->id;
                    if(m1==m2)
                    {
                        icount = icount + 1;
                        break;
                    }
                }
            }
            if(icount > 2)
                overlappingElementList.append(this->elements[i]);
        }
    }

    numOverlappingElements = overlappingElementList.length();

    if(numOverlappingElements>0)
    {
        this->error->setError(ERROR_DUPLICATEELEMENTS);
        return this->error->getError();
    }

    this->error->setError(ERROR_NOERROR);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that checks for overlapping boundary conditions
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::checkOverlappingBoundaries((int &numOverlappingBoundaries, QList<adcirc_node*> &overlappingBoundaryNodeList)
 * \brief Function that checks for overlapping boundary conditions in an adcirc_mesh
 *
 * @param[out] numOverlappingBoundaries Number of overlapping boundaries located
 * @param[out] overlappingBoundaryNodeList List of nodes involved in overlapping boundary conditions
 *
 * Function that checks for overlapping boundary conditions in an adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::checkOverlappingBoundaries(int &numOverlappingBoundaries, QList<adcirc_node*> &overlappingBoundaryNodeList)
{



    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to build the element table
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::buildElementTable()
 * \brief This function builds a table of the elements around a node
 *
 * This function uses the adcirc_node_table class to build a list of the elements around
 * a specific node, which is useful for many functions
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::buildElementTable()
{
    int i,j;

    this->node_table.clear();

    this->node_table.resize(this->numNodes);

    for(i=0;i<this->numNodes;i++)
        this->node_table[i] = new adcirc_node_table(this->nodes[i],this);

    for(i=0;i<this->numElements;i++)
        for(j=0;j<this->elements[i]->numConnections;j++)
            this->node_table[this->elements[i]->connections[j]->id-1]->add(this->elements[i]);

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that builds a nodal kdtree2
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::buildNodalSearchTree()
 * \brief Function that builds a kdtree2 search tree object for the nodes in the adcirc_mesh
 *
 * Function that builds a kdtree2 search tree object for the nodes in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::buildNodalSearchTree()
{
    int i,ierr;
    QVector<double> x,y;

    x.resize(this->numNodes);
    y.resize(this->numNodes);

    for(i=0;i<this->numNodes;i++)
    {
        x[i] = this->nodes[i]->position.x();
        y[i] = this->nodes[i]->position.y();
    }

    if(this->nodalSearchTree->initialized)
        delete this->nodalSearchTree;

    this->nodalSearchTree = new qKdtree2(this);
    ierr = this->nodalSearchTree->build(x,y);
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    this->nodalSearchTree->initialized = true;

    this->error->setError(ERROR_NOERROR);
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that builds a elemental kdtree2
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::buildElementalSearchTree()
 * \brief Function that builds a kdtree2 search tree object for the element centers in the adcirc_mesh
 *
 * Function that builds a kdtree2 search tree object for the element centers in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::buildElementalSearchTree()
{
    int i,j,ierr;
    QVector<double> x,y;

    x.resize(this->numElements);
    y.resize(this->numElements);

    for(i=0;i<this->numElements;i++)
    {
        x[i] = 0.0;
        y[i] = 0.0;
        for(j=0;j<this->elements[i]->numConnections;j++)
        {
            x[i] = x[i] + this->elements[i]->connections[j]->position.x();
            y[i] = y[i] + this->elements[i]->connections[j]->position.y();
        }
        x[i] = x[i] / static_cast<double>(this->elements[i]->numConnections);
        y[i] = y[i] / static_cast<double>(this->elements[i]->numConnections);
    }

    if(this->elementalSearchTree->initialized)
        delete this->elementalSearchTree;

    this->elementalSearchTree = new qKdtree2(this);
    ierr = this->elementalSearchTree->build(x,y);
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    this->elementalSearchTree->initialized = true;

    this->error->setError(ERROR_NOERROR);
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the nearest node
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::findNearestNode(QPointF pointLocation, adcirc_node* &nearestNode)
 * \brief Function that finds the nearest node to a specified x,y location in the adcirc_mesh
 *
 * @param[in]  pointLocation Location for which the function will determine the nearest node
 * @param[out] node          Pointer to the nearest node
 *
 * Function that finds the nearest node to a specified x,y location in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findNearestNode(QPointF pointLocation, adcirc_node* &nearestNode)
{
    int index;

    if(this->nodalSearchTree.isNull())
        this->nodalSearchTree = new qKdtree2(this);

    if(!this->nodalSearchTree->initialized)
        this->buildNodalSearchTree();

    this->nodalSearchTree->findNearest(pointLocation,index);

    nearestNode = this->nodes[index];

    this->error->setError(ERROR_NOERROR);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the nearest node
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::findNearestNode(double x, double y, adcirc_node* &nearestNode)
 * \brief Function that finds the nearest node to a specified x,y location in the adcirc_mesh
 *
 * @param[in]  x       x-coordinate to use for locating nearest node
 * @param[in]  y       y-coordinate to use for locating nearest node
 * @param[out] node    Pointer to the nearest node
 *
 * Function that finds the nearest node to a specified x,y location in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findNearestNode(double x, double y, adcirc_node* &nearestNode)
{
    int ierr = this->findNearestNode(QPointF(x,y),nearestNode);
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the list of nn nearest nodes
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::findXNearestNodes(QPointF pointLocation, int nn, QList<adcirc_node *> &nodeList)
 * \brief Function that finds a list of the nearest nodes to a specified x,y coordinate
 *
 * @param[in]  location point location to use for locating the nearest node
 * @param[in]  nn       Number of nearest nodes to find
 * @param[out] nodeList QList of pointers to the nn nearest nodes
 *
 * Function that finds the nearest node to a specified x,y location in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findXNearestNodes(QPointF pointLocation, int nn, QList<adcirc_node *> &nodeList)
{
    int i;
    QVector<int> indicies;

    if(this->nodalSearchTree.isNull())
        this->nodalSearchTree = new qKdtree2(this);

    if(!this->nodalSearchTree->initialized)
        this->buildNodalSearchTree();

    this->nodalSearchTree->findXNearest(pointLocation,nn,indicies);

    nodeList.clear();

    for(i=0;i<indicies.size();i++)
        nodeList.append(this->nodes[indicies[i]]);

    this->error->setError(ERROR_NOERROR);
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the list of nn nearest nodes
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::findXNearestNodes(double x, double y, int nn, QList<adcirc_node *> &nodeList)
 * \brief Function that finds a list of the nearest nodes to a specified x,y coordinate
 *
 * @param[in]  x        x-coordinate to use for locating nearest node
 * @param[in]  y        y-coordinate to use for locating nearest node
 * @param[in]  nn       Number of nearest nodes to find
 * @param[out] nodeList QList of pointers to the nn nearest nodes
 *
 * Function that finds the nearest node to a specified x,y location in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findXNearestNodes(double x, double y, int nn, QList<adcirc_node *> &nodeList)
{
    int ierr = this->findXNearestNodes(QPointF(x,y),nn,nodeList);
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the nn nearest elements
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::findXNearestElements(QPointF pointLocation, int nn, QList<adcirc_element*> &elementList)
 * \brief Function that finds a list of the nearest element to a specified x,y coordinate
 *
 * @param[in]  location    point location for locating the nearest element
 * @param[in]  nn          Number of nearest elements to find
 * @param[out] elementList QList of pointers to the nn nearest elements
 *
 * Function that finds the nearest element center to a specified x,y location in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findXNearestElements(QPointF pointLocation, int nn, QList<adcirc_element*> &elementList)
{
    int i;
    QVector<int> indicies;

    if(this->elementalSearchTree.isNull())
        this->elementalSearchTree = new qKdtree2(this);

    if(!this->elementalSearchTree->initialized)
        this->buildElementalSearchTree();

    this->elementalSearchTree->findXNearest(pointLocation,nn,indicies);

    elementList.clear();

    for(i=0;i<indicies.size();i++)
        elementList.append(this->elements[indicies[i]]);

    this->error->setError(ERROR_NOERROR);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the nn nearest elements
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::findXNearestElements(double x, double y, int nn, QList<adcirc_element*> &elementList)
 * \brief Function that finds a list of the nearest element to a specified x,y coordinate
 *
 * @param[in]  x           x-coordinate to use for locating nearest element
 * @param[in]  y           y-coordinate to use for locating nearest element
 * @param[in]  nn          Number of nearest elements to find
 * @param[out] elementList QList of pointers to the nn nearest elements
 *
 * Function that finds the nearest element center to a specified x,y location in the adcirc_mesh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findXNearestElements(double x, double y, int nn, QList<adcirc_element*> &elementList)
{
    int ierr = this->findXNearestElements(QPointF(x,y),nn,elementList);
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the element that a given x,y resides in
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::findElement(double x, double y, adcirc_element* &nearestElement, bool &found, QVector<double> &weights)
 * \brief Function that locates the element that a point resides within and returns interpolation weights
 *
 * @param[in]  pointLocation   point to use for locating the element
 * @param[out] nearestElement  pointer to element that point resides within or the nearest element
 * @param[out] found           true if the point was found within an element. false if point
 *                             not located within an element and the nearest element was returned
 * @param[out] weights         Vector of interpolation weights for the given point on the returned element
 *
 * Function that locates the element that a point resides within and returns interpolation weights
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findElement(QPointF pointLocation, adcirc_element* &nearestElement, bool &found, QVector<double> &weights)
{
    int ierr = this->findAdcircElement(pointLocation,nearestElement,found,weights);
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the element that a given x,y resides in
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::findElement(double x, double y, adcirc_element* &nearestElement, bool &found)
 * \brief Function that locates the element that a point resides within
 *
 * @param[in]  x               x-coordinate to use for locating the element
 * @param[in]  y               y-coordinate to use for locating the element
 * @param[out] nearestElement  pointer to element that point resides within or the nearest element
 * @param[out] found           true if the point was found within an element. false if point
 *                             not located within an element and the nearest element was returned
 *
 * Function that locates the element that a point resides within
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findElement(double x, double y, adcirc_element* &nearestElement, bool &found)
{
    QVector<double> dmy;
    int ierr = this->findAdcircElement(QPointF(x,y),nearestElement,found,dmy);
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the element that a given x,y resides in
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::findElement(QPointF pointLocation, adcirc_element* &nearestElement, bool &found)
 * \brief Function that locates the element that a point resides within
 *
 * @param[in]  pointLocation   point to use for locating the element
 * @param[out] nearestElement  pointer to element that point resides within or the nearest element
 * @param[out] found           true if the point was found within an element. false if point
 *                             not located within an element and the nearest element was returned
 *
 * Function that locates the element that a point resides withinh
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findElement(QPointF pointLocation, adcirc_element* &nearestElement, bool &found)
{
    QVector<double> dmy;
    int ierr = this->findAdcircElement(pointLocation,nearestElement,found,dmy);
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function that finds the element that a given x,y resides in
//-----------------------------------------------------------------------------------------//
/**
 * \overload adcirc_mesh::findElement(double x, double y, adcirc_element* &nearestElement, bool &found, QVector<double> &weights)
 * \brief Function that locates the element that a point resides within
 *
 * @param[in]  x               x-coordinate to use for locating the element
 * @param[in]  y               y-coordinate to use for locating the element
 * @param[out] nearestElement  pointer to element that point resides within or the nearest element
 * @param[out] found           true if the point was found within an element. false if point
 *                             not located within an element and the nearest element was returned
 * @param[out] weights         Vector of interpolation weights for the given point on the returned element
 *
 * Function that locates the element that a point resides within and returns interpolation weights
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findElement(double x, double y, adcirc_element* &nearestElement, bool &found, QVector<double> &weights)
{
    int ierr = this->findAdcircElement(QPointF(x,y),nearestElement,found,weights);
    this->error->setError(ierr);
    return this->error->getError();
}
//-----------------------------------------------------------------------------------------//


int adcirc_mesh::toShapefile(QString filename)
{

    SHPHandle shpid;
    DBFHandle dbfid;
    SHPObject *shpobj;
    int i,shp_index,nodeid;
    double latitude,longitude,elevation;

    shpid = SHPCreate(filename.toStdString().c_str(),SHPT_POINT);
    dbfid = DBFCreate(filename.toStdString().c_str());

    DBFAddField(dbfid,"nodeid",FTInteger,16,0);
    DBFAddField(dbfid,"longitude",FTDouble,16,8);
    DBFAddField(dbfid,"latitude",FTDouble,16,8);
    DBFAddField(dbfid,"elevation",FTDouble,16,4);

    for(i=0;i<this->numNodes;i++)
    {

        nodeid = this->nodes[i]->id;
        longitude = static_cast<double>(this->nodes[i]->position.x());
        latitude = static_cast<double>(this->nodes[i]->position.y());
        elevation = static_cast<double>(this->nodes[i]->position.z());

        shpobj = SHPCreateSimpleObject(SHPT_POINT,1,&longitude,&latitude,&elevation);
        shp_index = SHPWriteObject(shpid,-1,shpobj);
        SHPDestroyObject(shpobj);

        DBFWriteIntegerAttribute(dbfid,shp_index,0,nodeid);
        DBFWriteDoubleAttribute(dbfid,shp_index,1,longitude);
        DBFWriteDoubleAttribute(dbfid,shp_index,2,latitude);
        DBFWriteDoubleAttribute(dbfid,shp_index,3,elevation);

    }

    SHPClose(shpid);

    return ERROR_NOERROR;

}




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
/**
 * \fn adcirc_mesh::readMesh()
 * \brief This function is used internally to read an ADCIRC mesh
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
        this->nodeToPositionMapping[this->nodes[i]->id] = i;
        this->nodeToIdMapping[i] = this->nodes[i]->id;
    }

    //...Loop over the elements
    for(i=0;i<this->numElements;i++)
    {
        ierr = this->elements[i]->fromString(meshFileList.value(this->numNodes+2+i),this->nodes,this->nodeToPositionMapping);
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
        this->elementToPositionMapping[this->elements[i]->id] = i;
        this->elementToIdMapping[i] = this->elements[i]->id;
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

    //...Build the element table, only if nodes are sequential
    if(!this->meshNeedsNumbering)
        this->buildElementTable();

    //...The mesh read is now complete. We're done.

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Allocates an array of pointers in the ADCIRC mesh
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::allocateNodes()
 * \brief Creates a number of adcirc_node variables on the heap
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
/**
 * \fn adcirc_mesh::allocateElements()
 * \brief Creates a number of adcirc_element variables on the heap
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
/**
 * \fn adcirc_mesh::readOpenBoundaries(int &position, QStringList &fileData)
 * \brief Protected function to read the entire set of open boundary conditions
 *
 * @param[in,out] position The current file position. Returned as the new file position when the open boundary read is complete
 * @param[in]     fileData Reference to the data read from the ADCIRC mesh file
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
            ierr = this->openBC[i]->fromString(tempString,j,this->nodes,this->nodeToPositionMapping);
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
/**
 * \fn adcirc_mesh::readLandBoundaries(int &position, QStringList &fileData)
 * \brief Protected function to read the entire set of land boundary conditions
 *
 * @param[in,out] position The current file position. Returned as the new file position
 *                         when the land boundary read is complete
 * @param[in]     fileData Reference to the data read from the ADCIRC mesh file
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
            ierr = this->landBC[i]->fromString(tempString,j,this->nodes,this->nodeToPositionMapping);
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
/**
 * \fn adcirc_mesh::writeMesh(QString filename)
 * \brief This function is used internally to write an ADCIRC mesh
 *
 * @param[in] filename Name of the output mesh file
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
        out << this->nodes[i]->toString(this->isLatLon) << "\n";

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



//-----------------------------------------------------------------------------------------//
//...Function to find the adcirc_element that a given x,y lies within
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::findAdcircElement(QPointF location, adcirc_element* &nearestElement, bool &found, QVector<double> &weights)
 * \brief Function used internally to find the adcirc_element that a given x,y lies within
 *
 * @param[in]  pointLocation  point to locate inside an element
 * @param[out] nearestElement pointer to the nearest adcirc_element to the given x,y
 * @param[out] found          true if the x,y coordinate was found within an element or false
 *                            if the location is not within any adcirc_element in which case the
 *                            nearest element is given
 * @param[out] weights        vector containing the weighting function for the given x,y inside the
 *                            adcirc_element. Used to weight interpolation functions.
 *
 * Function used internally to find the adcirc_element that a given x,y lies within and
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::findAdcircElement(QPointF pointLocation, adcirc_element* &nearestElement, bool &found, QVector<double> &weights)
{
    int i;
    double x,y;
    double x1,x2,x3;
    double y1,y2,y3;
    double sa1,sa2,sa3,ta;
    int searchDepth = 20;
    adcirc_node *n1,*n2,*n3;
    adcirc_element *e;
    QList<adcirc_element*> elementList;

    x = static_cast<double>(pointLocation.x());
    y = static_cast<double>(pointLocation.y());

    this->findXNearestElements(pointLocation,searchDepth,elementList);

    found = false;
    weights.resize(3);

    for(i=0;i<searchDepth;i++)
    {
        e   = elementList.value(i);
        n1  = e->connections[0];
        n2  = e->connections[1];
        n3  = e->connections[2];
        x1  = n1->position.x();
        x2  = n2->position.x();
        x3  = n3->position.x();
        y1  = n1->position.y();
        y2  = n2->position.y();
        y3  = n3->position.y();

        sa1 = qAbs( (x2*y3-x3*y2) - (x*y3-x3*y)   + (x*y2-x2*y)   );
        sa2 = qAbs( (x*y3-x3*y)   - (x1*y3-x3*y1) + (x1*y-x*y1)   );
        sa3 = qAbs( (x2*y-x*y2)   - (x1*y-x*y1)   + (x1*y2-x2*y1) );
        ta  = qAbs( (x2*y3-x3*y2) - (x1*y3-x3*y1) + (x1*y2-x2*y1) );

        if(sa1+sa2+sa3 < 1.001*ta)
        {
            nearestElement = e;
            found = true;
            weights[0] = ((x-x3)*(y2-y3)+(x2-x3)*(y3-y))/ta;
            weights[1] = ((x-x1)*(y3-y1)-(y-y1)*(x3-x1))/ta;
            weights[2] = ((y-y1)*(x2-x1)-(x-x1)*(y2-y1))/ta;
            return ERROR_NOERROR;
        }

    }

    if(!found)
    {
        nearestElement = elementList.value(0);
        weights.fill(1.0/3.0);
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Function to read an ADCIRC mesh from netCDF
//-----------------------------------------------------------------------------------------//
/**
 * \fn adcirc_mesh::readMeshFromNetCDF()
 * \brief This function is used internally to read an ADCIRC mesh from a netCDF file
 *
 * This function is used internally to read an ADCIRC mesh from a netCDF file. The mesh
 * will have its filename specified in the adcirc_mesh::filename variable.
 *
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_mesh::readMeshFromNetCDF()
{
    int i,ierr;
    int n1,n2,n3;
    int ncid,varid_x,varid_y,varid_z,varid_element;
    int dimid_nNodes,dimid_nElements,dimid_nVertex;
    double *node_x,*node_y,*node_z;
    int *elem1,*elem2,*elem3;
    size_t nNodes,nEle,nVert;

    //...Open the netCDF file
    ierr = nc_open(this->filename.toStdString().c_str(),NC_NOWRITE,&ncid);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    //...Get the variable IDs
    ierr = nc_inq_varid(ncid,"x",&varid_x);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_varid(ncid,"y",&varid_y);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_varid(ncid,"depth",&varid_z);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_varid(ncid,"element",&varid_element);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    //...Get the variable dimensions
    ierr = nc_inq_dimid(ncid,"node",&dimid_nNodes);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_dimid(ncid,"nele",&dimid_nElements);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_dimid(ncid,"nvertex",&dimid_nVertex);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_dimlen(ncid,dimid_nNodes,&nNodes);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_dimlen(ncid,dimid_nElements,&nEle);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_inq_dimlen(ncid,dimid_nVertex,&nVert);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    //...Save off the dimensional information
    this->numNodes          = static_cast<int>(nNodes);
    this->numElements       = static_cast<int>(nEle);
    this->numOpenBoundaries = 0;
    this->numLandBoundaries = 0;
    const size_t count_node = nNodes;

    static size_t count_ele[]  = {nEle,1};
    static size_t start_ele1[] = {0,0};
    static size_t start_ele2[] = {0,1};
    static size_t start_ele3[] = {0,2};

    //...Allocate space for reading from netCDF
    node_x = (double*)malloc(sizeof(double)*count_node);
    node_y = (double*)malloc(sizeof(double)*count_node);
    node_z = (double*)malloc(sizeof(double)*count_node);
    elem1  = (int*)malloc(sizeof(int)*nEle);
    elem2  = (int*)malloc(sizeof(int)*nEle);
    elem3  = (int*)malloc(sizeof(int)*nEle);

    //...Create the nodes
    ierr = this->allocateNodes();
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    //...Create the elements
    ierr = this->allocateElements();
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    //...Read the data from the netCDF file
    ierr = nc_get_var(ncid,varid_x,node_x);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_get_var(ncid,varid_y,node_y);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_get_var(ncid,varid_z,node_z);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_get_vara_int(ncid,varid_element,start_ele1,count_ele,elem1);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_get_vara_int(ncid,varid_element,start_ele2,count_ele,elem2);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    ierr = nc_get_vara_int(ncid,varid_element,start_ele3,count_ele,elem3);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    //...Close the netCDF file
    ierr = nc_close(ncid);
    if(ierr!=NC_NOERR)
    {
        this->error->setCustomDescription(nc_strerror(ierr));
        this->error->setError(ERROR_NETCDF_GENERIC);
        return this->error->getError();
    }

    //...Save the information read into adcirc_node objects
    for(i=0;i<this->numNodes;i++)
    {
        this->nodes[i]->id = i+1;
        this->nodes[i]->position.setX(node_x[i]);
        this->nodes[i]->position.setY(node_y[i]);
        this->nodes[i]->position.setZ(node_z[i]);
    }

    //...Save the information read into adcirc_element objects
    for(i=0;i<this->numElements;i++)
    {
        n1 = elem1[i]-1;
        n2 = elem2[i]-1;
        n3 = elem3[i]-1;

        this->elements[i]->id = i+1;
        this->elements[i]->connections[0] = this->nodes.at(n1);
        this->elements[i]->connections[1] = this->nodes.at(n2);
        this->elements[i]->connections[2] = this->nodes.at(n3);
    }

    //...Deallocate the memory
    free(node_x);
    free(node_y);
    free(node_z);
    free(elem1);
    free(elem2);
    free(elem3);

    //...Build the element table
    ierr = this->buildElementTable();
    if(ierr!=ERROR_NOERROR)
    {
        this->error->setError(ierr);
        return this->error->getError();
    }

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
