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
#include "qkdtree2.h"
#include "boost/array.hpp"
#include "boost/multi_array.hpp"
#include <QDebug>

qKdtree2::qKdtree2(QObject *parent) : QObject(parent)
{
    this->initialized = false;
    this->numDataPoints = 0;
}

int qKdtree2::build(QVector<double> &x, QVector<double> &y)
{
    int i;
    typedef boost::multi_array<float,2> array2d;

    if(x.size()!=y.size())
        return ERROR_QKDTREE2_SIZEMISMATCH;

    this->numDataPoints = x.size();

    array2d data(boost::extents[x.size()][2]);

    for(i=0;i<x.size();i++)
    {
        data[i][0] = static_cast<float>(x[i]);
        data[i][1] = static_cast<float>(y[i]);
    }

    this->tree = QSharedPointer<kdtree2>(new kdtree2(data,true));

    return ERROR_NOERROR;
}

int qKdtree2::findNearest(double x, double y, int &index)
{

    int ierr;

    this->findNearest(QPointF(x,y),index);

    return ierr;
}



int qKdtree2::findNearest(QPointF location, int &index)
{
    kdtree2_result_vector result_vector;
    kdtree2_result        result;
    vector<float>         query(2);

    query[0] = static_cast<float>(location.x());
    query[1] = static_cast<float>(location.y());

    this->tree->n_nearest(query,1,result_vector);

    result = result_vector.at(0);
    index  = result.idx;

    return ERROR_NOERROR;
}



int qKdtree2::findXNearest(double x, double y, int nn, QVector<int> &indicies)
{
    int ierr;

    ierr = this->findXNearest(QPointF(x,y),nn,indicies);

    return ierr;
}



int qKdtree2::findXNearest(QPointF location, int nn, QVector<int> &indicies)
{
    int                   i;
    kdtree2_result_vector result_vector;
    kdtree2_result        result;
    vector<float>         query(2);

    query[0] = static_cast<float>(location.x());
    query[1] = static_cast<float>(location.y());

    this->tree->n_nearest(query,1,result_vector);

    indicies.resize(nn);

    for(i=0;i<nn;i++)
    {
        result      = result_vector.at(0);
        indicies[i] = result.idx;
    }

    return ERROR_NOERROR;
}

