#include "qkdtree2.h"
#include "boost/array.hpp"
#include "boost/multi_array.hpp"

qKdtree2::qKdtree2(QObject *parent) : QObject(parent)
{
    this->numDataPoints = 0;
}

qKdtree2::~qKdtree2()
{
    delete this->tree;
}

int qKdtree2::build(QVector<double> &x, QVector<double> &y)
{
    int i;
    typedef boost::multi_array<float,2> array2d;

    if(x.size()!=y.size())
        return -1;

    this->numDataPoints = x.size();

    array2d data(boost::extents[x.size()][2]);

    for(i=0;i<x.size();i++)
    {
        data[i][0] = static_cast<float>(x[i]);
        data[i][1] = static_cast<float>(y[i]);
    }

    this->tree = new kdtree2(data,true);

    return 0;
}

int qKdtree2::findNearest(double x, double y, int &index)
{
    kdtree2_result_vector result_vector;
    kdtree2_result        result;
    vector<float>         query(2);

    query[0] = static_cast<float>(x);
    query[1] = static_cast<float>(y);

    this->tree->n_nearest(query,1,result_vector);

    result = result_vector.at(0);
    index  = result.idx;

    return 0;
}

int qKdtree2::findXNearest(double x, double y, int nn, QVector<int> &indicies)
{
    int                   i;
    kdtree2_result_vector result_vector;
    kdtree2_result        result;
    vector<float>         query(2);

    query[0] = static_cast<float>(x);
    query[1] = static_cast<float>(y);

    this->tree->n_nearest(query,1,result_vector);

    indicies.resize(nn);

    for(i=0;i<nn;i++)
    {
        result      = result_vector.at(0);
        indicies[i] = result.idx;
    }

    return 0;
}
