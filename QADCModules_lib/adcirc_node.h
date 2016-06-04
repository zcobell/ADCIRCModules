/*-------------------------------GPL-------------------------------------//
//
// QADCModules - A library for working with ADCIRC models
// Copyright (C) 2015  Zach Cobell
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
/**
 * \class adcirc_node
 *
 * \brief Class that describes an ADCIRC node
 *
 * This class describes a single adcirc node. Functions are provided
 * to read/write a node in the format used in an ADCIRC mesh file
 *
 * \author Zach Cobell
 *
 * Contact: zcobell@gmail.com
 *
 */
#ifndef ADCIRC_NODE_H
#define ADCIRC_NODE_H

#include <QVector>
#include <QObject>
#include <QStringList>
#include <QVector3D>
#include <QCryptographicHash>

#include "QADCModules_global.h"
#include "adcirc_forward.h"

class QADCMODULESSHARED_EXPORT adcirc_node : public QObject
{
    Q_OBJECT
public:

    explicit adcirc_node(QObject *parent = 0);

    ///Position in space of the adcirc node (x,y)
    QVector3D position;

    ///ID for the ADCIRC node
    int    id;

    ///Boolean value that can be used during checking operations
    bool myBool;

    ///Cryptogrpahic hash for this adcirc node (x,y position only)
    QString positionHash;

    ///Cryptographic hash for this adcirc node (x,y and z)
    QString fullHash;

    ///Nodal Attribute data from a fort.13
    QVector<adcirc_nodalattribute*> nodalData;

    ///Nodal parameters for this node
    //QVector<adcirc_nodalattribute*> nodalAttributes;


    //...PUBLIC FUNCTIONS...//

    int fromString(QString line);

    QString toString(bool gcs = true);

    int hashNode();

    QPointF toPointF();

    int setHashAlgorithm(QCryptographicHash::Algorithm hashType);

private:

    ///Hash algorithm to used (Default: Sha1)
    QCryptographicHash::Algorithm hashAlgorithm;

};

//...OPERATORS...//
bool operator<(const adcirc_node n1,const adcirc_node n2);
bool operator>(const adcirc_node n1,const adcirc_node n2);

#endif // ADCIRC_NODE_H
