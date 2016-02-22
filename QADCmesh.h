/**
 * \class QADCmesh
 *
 * \addtogroup QADCModules
 *
 * \brief A class for reading/writing ADCIRC meshes
 *
 * \author Zachary Cobell
 * \version Version 0.1
 * \date 02/21/2016
 *
 * Contact: zcobell@gmail.com
 *
 * Created on: 02/21/2016
 *
 */
#ifndef QADCMESH_H
#define QADCMESH_H

#include <QObject>
#include <QVector>
#include <QFile>
#include "QADCModules_flags.h"

class QADCmesh : public QObject
{
    Q_OBJECT
public:
    ///
    explicit QADCmesh(QObject *parent = 0);



    //...STRUCTURES USED FOR ADCIRC MESHES....//

    //...ADCIRC Boundary Listing
    /** This struct specifies an ADCIRC boundary listing for use
     *  when working with ADCIRC meshes
     **/
    struct adcirc_boundary_listing
    {
        int             numNodes;       ///Number of nodes on the boundary string
        int             code;           ///ADCIRC model boundary code
        QVector<int>    n1;             ///First node in the boundary
        QVector<int>    n2;             ///Second node in the boundary. Used for weirs where boundary conditions have an opposite side
        QVector<double> crest;          ///Boundary crest. Used for weir type boundaries
        QVector<double> supercritical;  ///Coefficient of supercritical flow over a weir
        QVector<double> subcritical;    ///Coefficient of subcritical flow over a weir
        QVector<double> pipeHeight;     ///Height of the pipe/culvert in a weir
        QVector<double> pipeDiam;       ///Diameter of the pipe/culvert in a weir
        QVector<double> pipeCoef;       ///Pipe coefficient for the pipe/culvert in a weir
    };

    //...ADCIRC Node
    /** This struct specifies the positional information for
     * a single ADCIRC node
    */
    struct adcirc_node
    {
        int    id;
        double x;
        double y;
        double z;
    };

    //...ADCIRC Element
    /** This struct specifies the connection informtion to form
     * a triangular ADCIRC element. Note that the numConnections
     * variable is always 3 since ADCIRC does not use other geometries
    */
    struct adcirc_element
    {
        int id;
        int numConnections;
        int node1;
        int node2;
        int node3;
    };

    //...Container for an ADCIRC mesh
    ///This struct specifies the container for an ADCIRC mesh
    struct adcirc_mesh
    {
        QString                            title;
        int                                numElements;
        int                                numNodes;
        QVector<adcirc_node>               nodes;
        QVector<adcirc_element>            elements;
        int                                numOpenBoundaries;
        int                                totNumOpenBoundaryNodes;
        int                                numLandBonudaries;
        int                                totNumLandBoundaryNodes;
        QVector<adcirc_boundary_listing>   oceanBC;
        QVector<adcirc_boundary_listing>   landBC;
    };


    //...Public Functions
    int readADCIRCMesh(QString filename,adcirc_mesh &myMesh);

signals:

public slots:

private:
    //...Private Functions
    int readADCIRCNode(QString line, int index, adcirc_node &node);
    int readADCIRCElement(QString line, int index, adcirc_element &element);

    //...Private Variables
    int errorCode;
};

#endif // QADC_MESH_H
