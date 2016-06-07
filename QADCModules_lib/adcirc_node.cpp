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
#include "adcirc_node.h"
#include "QADCModules_errors.h"


//-----------------------------------------------------------------------------------------//
// Comparison operator for adcirc_node object: less than
//-----------------------------------------------------------------------------------------//
/** \brief Comparison operator for adcirc_node object: less than
 *
 * @param[in] n1 node1 for the comparison
 * @param[in] n2 node2 for the comparison
 *
 * Comparison operator for adcirc_node object: less than. Checks if node ID 1 is less than
 * node ID 2.
 *
 */
//-----------------------------------------------------------------------------------------//
bool adcirc_node::nodeLessThan(const adcirc_node n1,const adcirc_node n2)
{
    return n1.id<n2.id;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Comparison operator for adcirc_node object: greater than
//-----------------------------------------------------------------------------------------//
/** \brief Comparison operator for adcirc_node object: greater than
 *
 * @param[in] n1 node1 for the comparison
 * @param[in] n2 node2 for the comparison
 *
 * Comparison operator for adcirc_node object: greater than. Checks if node ID 1 is greater than
 * node ID 2.
 *
 */
//-----------------------------------------------------------------------------------------//
bool adcirc_node::nodeGreaterThan(const adcirc_node n1,const adcirc_node n2)
{
    return n1.id>n2.id;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Constructor
//-----------------------------------------------------------------------------------------//
/** \brief Constructor function for an adcirc_node
 *
 * @param[in] parent QObject reference
 *
 * Constructor function for an adcirc_node
 */
//-----------------------------------------------------------------------------------------//
adcirc_node::adcirc_node(QObject *parent) : QObject(parent)
{
    this->hashAlgorithm = QCryptographicHash::Sha1;
    this->id = 0;
    this->myBool = false;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
//...Parses a line from the ADCIRC mesh that contains a node into
//   its positional information. Note that if the mesh needs re-numbering,
//   you will need to set ignoreMeshNumbering to true
//-----------------------------------------------------------------------------------------//
/** \brief Protected function to parse the string from an ADCIRC mesh file containing nodal information
 *
 * @param[in] line The QString with the information read from the file to be parsed into an ADCIRC node
 *
 * This function can parse a single line from an ADCIRC mesh file to break it into the nodal information
 **/
//-----------------------------------------------------------------------------------------//
int adcirc_node::fromString(QString line)
{
    QStringList tempList;
    QString     tempString;
    int         tempInt;
    qreal      tempDouble;
    bool        err;

    tempList   = line.simplified().split(" ");
    tempString = tempList.value(0);
    tempInt    = tempString.toInt(&err);
    if(!err)
        return ERROR_MESHREAD_NODERR;
    this->id = tempInt;

    tempString = tempList.value(1);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_NODERR;
    this->position.setX(tempDouble);
    tempString = tempList.value(2);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_NODERR;
    this->position.setY(tempDouble);
    tempString = tempList.value(3);
    tempDouble = tempString.simplified().toDouble(&err);
    if(!err)
        return ERROR_MESHREAD_NODERR;
    this->position.setZ(tempDouble);

    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function that writes an ADCIRC node to a string
//-----------------------------------------------------------------------------------------//
/** \brief Protected function that writes an ADCIRC node in the format expected by an ADCIRC mesh
 * file
 *
 * @param[in] gcs  Determines the output format if it should be formatted for geographic
 *                 coordinates or mercator based coordinates. Default = true
 *
 * Protected function that writes an ADCIRC node in the format expected by an ADCIRC mesh
 * file
 */
//-----------------------------------------------------------------------------------------//
QString adcirc_node::toString(bool gcs)
{
    QString output;

    if(gcs)
        output.sprintf("%11i   %14.10f   %14.10f  %14.10f",this->id,this->position.x(),this->position.y(),this->position.z());
    else
        output.sprintf("%11i   %14.4f    %14.4f   %14.4f",this->id,this->position.x(),this->position.y(),this->position.z());
    return output;
}
//-----------------------------------------------------------------------------------------//


//-----------------------------------------------------------------------------------------//
// Function that generates a unique hash for this adcirc_node
//-----------------------------------------------------------------------------------------//
/** \brief Function that generates a unique hash for this adcirc_node
 *
 * Function that generates a unique hash for this adcirc_node
 */
//-----------------------------------------------------------------------------------------//
int adcirc_node::hashNode()
{
    QString hashSeed1,hashSeed2,hashSeed3;

    //...initialize the sha1 hash
    QCryptographicHash localHash(this->hashAlgorithm);
    localHash.reset();

    //...Create a formatted string for each x, y and z;
    hashSeed1 = QString();
    hashSeed2 = QString();
    hashSeed3 = QString();
    hashSeed1.sprintf("%+018.12e",this->position.x());
    hashSeed2.sprintf("%+018.12e",this->position.y());
    hashSeed3.sprintf("%+018.12e",this->position.z());

    //...Create the hash for the local node
    localHash.addData(hashSeed1.toUtf8(),hashSeed1.length());
    localHash.addData(hashSeed2.toUtf8(),hashSeed2.length());

    //...save the x,y node hash
    this->positionHash = localHash.result().toHex();

    //...Save the hash that includes the nodal elevation
    localHash.addData(hashSeed3.toUtf8(),hashSeed3.length());
    this->fullHash = localHash.result().toHex();

    return 0;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function that converts a node to a QPointF (x,y)
//-----------------------------------------------------------------------------------------//
/** \brief Function that converts a node to a QPointF (x,y)
 *
 * Function that converts a node to a QPointF (x,y)
 */
//-----------------------------------------------------------------------------------------//
QPointF adcirc_node::toPointF()
{
    QPointF point;
    point.setX(this->position.x());
    point.setY(this->position.y());
    return point;
}
//-----------------------------------------------------------------------------------------//



//-----------------------------------------------------------------------------------------//
// Function to set the type of hash that is to be used for this class
//-----------------------------------------------------------------------------------------//
/** \brief Public function to set the hash algorithm to use
 *
 * @param[in] hashType  The QCryptographicHash::Algorithm to use
 *
 * This function sets the type of hash algorithm to use for hashes constructed for this
 * class. The hash can be MD4, MD5, SHA1, SSA256, SHA512 or any other contained with Qt
 * The default is Sha1
 *
 */
//-----------------------------------------------------------------------------------------//
int adcirc_node::setHashAlgorithm(QCryptographicHash::Algorithm hashType)
{
    this->hashAlgorithm = hashType;
    return ERROR_NOERROR;
}
//-----------------------------------------------------------------------------------------//
