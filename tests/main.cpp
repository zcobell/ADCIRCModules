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

#include <QCoreApplication>
#include <QPointer>
#include <QDebug>
#include "QADCModules.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    int i,ierr;
    int numLeveesRaised,numDisjoint,numDuplicates;
    double maxLeveeRaise;
    adcirc_node* nearestNode;
    adcirc_element* nearestElement;
    QList<adcirc_node*> disjointList;
    QList<adcirc_element*> duplicateElements;
    QVector<double> weight;
    bool elementFound;

    ierr = 0;

    //...adcirc_mesh testing..//
    QPointer<adcirc_mesh> thisMesh = new adcirc_mesh();

    qDebug() << "Reading mesh test...";
    ierr = thisMesh->read("../../QADCModules/tests/test_files/ms-riv.grd");
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();

    if(thisMesh->error->getError()!=ERROR_NOERROR)
    {
        qDebug() << "Mesh read test failed.";
        return ERROR_NOERROR;
    }

    qDebug() << "\n";
    qDebug() << "Writing mesh test...";
    ierr = thisMesh->write("../../QADCModules/tests/test_files/ms-riv-me.grd");
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();

    if(thisMesh->error->getError()!=ERROR_NOERROR)
    {
        qDebug() << "Mesh write test failed.";
        return ERROR_NOERROR;
    }


    qDebug() << "\n";
    qDebug() << "Checking levee heights test...";
    ierr = thisMesh->checkLeveeHeights();
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();

    if(ierr!=ERROR_LEVEE_BELOWTOPO)
    {
        qDebug() << "ERROR: Failed to detect invalid levee height";
        return thisMesh->error->getError();
    }
    else
        qDebug() << "Low levees were correctly detected.";

    //...Reset the expected error;
    thisMesh->error->resetError();

    qDebug() << "\n";
    qDebug() << "Updating levee heights test...";
    ierr = thisMesh->raiseLeveeHeights(numLeveesRaised,maxLeveeRaise,0.20,0.01,"../../QADCModules/tests/test_files/levee_check.txt");
    qDebug() << "Number of levees raised: " << numLeveesRaised;
    qDebug() << "Maximum amount levee raised: " << maxLeveeRaise;
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();


    qDebug() << "\n";
    qDebug() << "Checking disjoint nodes test...";
    ierr = thisMesh->checkDisjointNodes(numDisjoint,disjointList);
    qDebug() << "Number of disjoint nodes: " << numDisjoint;
    for(i=0;i<disjointList.size();i++)
        qDebug() << disjointList.at(i)->toString(thisMesh->isLatLon);
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();
    thisMesh->error->resetError();


    qDebug() << "\n";
    qDebug() << "Eliminating disjoint nodes test...";
    ierr = thisMesh->eliminateDisjointNodes(numDisjoint);
    qDebug() << "Number of disjoint nodes eliminated: " << numDisjoint;
    ierr = thisMesh->write("../../QADCModules/tests/test_files/ms-riv-me-disjoint.grd");
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();
    thisMesh->error->resetError();


    qDebug() << "\n";
    qDebug() << "Renumbering mesh test...";
    ierr = thisMesh->renumber();
    ierr = thisMesh->write("../../QADCModules/tests/test_files/ms-riv-me-renumber.grd");
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();
    if(thisMesh->error->getError()!=ERROR_NOERROR)
        return thisMesh->error->getError();


    qDebug() << "\n";
    qDebug() << "Duplicate elements test...";
    ierr = thisMesh->checkOverlappingElements(numDuplicates,duplicateElements);
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();
    if(thisMesh->error->getError()!=ERROR_NOERROR)
        return thisMesh->error->getError();


    qDebug() << "\n";
    qDebug() << "Testing KDTREE2...";
    ierr = thisMesh->findNearestNode(-90.478577,30.036816,nearestNode);
    qDebug() << "Nearest Location Node Number: " << nearestNode->id;
    qDebug() << "STATUS: " << ierr;
    ierr = thisMesh->findElement(-90.477689,30.040329,nearestElement,elementFound,weight);
    qDebug() << "Element ID: " << nearestElement->id;
    qDebug() << "Element Found: " << elementFound;
    qDebug() << "Weight: " << weight[0] << weight[1] << weight[2] << weight[0]+weight[1]+weight[2];
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();
    if(thisMesh->error->getError()!=ERROR_NOERROR)
        return thisMesh->error->getError();


    qDebug() << "\n";
    qDebug() << "Testing Shapelib conversion...";
    ierr = thisMesh->toShapefile("../../QADCModules/tests/test_files/ms-riv.shp");
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();
    if(thisMesh->error->getError()!=ERROR_NOERROR)
        return thisMesh->error->getError();


    qDebug() << "\n";
    qDebug() << "Testing Proj4 Coordinate Transformation...";
    ierr = thisMesh->project(26915);
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();
    if(thisMesh->error->getError()!=ERROR_NOERROR)
        return thisMesh->error->getError();
    if(thisMesh->error->getError()!=ERROR_NOERROR)
        return thisMesh->error->getError();
    ierr = thisMesh->write("../../QADCModules/tests/test_files/ms-riv-me-utm15.grd");


    //...nodal_attributes, nodal_parameter testing...//
    qDebug() << "\n";
    qDebug() << "Reading Fort.13 test...";
    QPointer<adcirc_nodalattributes> thisNodalParam = new adcirc_nodalattributes();
    ierr = thisNodalParam->read("../../QADCModules/tests/test_files/ms-riv.13");
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();

    if(thisNodalParam->error->getError()!=ERROR_NOERROR)
    {
        qDebug() << "Fort13 read test failed.";
        return thisNodalParam->error->getError();
    }

    qDebug() << "\n";
    qDebug() << "Writing fort.13 test...";
    ierr = thisNodalParam->write("../../QADCModules/tests/test_files/ms-riv-me.13");
    qDebug() << "STATUS: " << thisMesh->error->getErrorString();

    if(thisNodalParam->error->getError()!=ERROR_NOERROR)
    {
        qDebug() << "Fort13 write test failed.";
        return thisNodalParam->error->getError();
    }


    //...adcirc_output testing...//
    qDebug() << "\n";
    qDebug() << "Reading adcirc global netCDF file...";
    QPointer<adcirc_global_output> adcOutput = new adcirc_global_output("../../QADCModules/tests/test_files/maxele.63.nc");
    ierr = adcOutput->readNetCDF(1);
    qDebug() << "STATUS: " << adcOutput->error->getErrorString();
    if(adcOutput->error->getError()!=ERROR_NOERROR)
    {
        qDebug() << "adcirc global netCDF read failed.";
        return adcOutput->error->getError();
    }

    qDebug() << "\n";
    qDebug() << "Reading adcirc global ascii file...";
    adcOutput.clear();
    adcOutput = new adcirc_global_output("../../QADCModules/tests/test_files/maxele.63");
    ierr = adcOutput->readAscii();
    qDebug() << "STATUS: " << adcOutput->error->getErrorString();
    if(adcOutput->error->getError()!=ERROR_NOERROR)
    {
        qDebug() << "adcirc global ascii read failed.";
        return adcOutput->error->getError();
    }


    return 0;
}
