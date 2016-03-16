#ifndef QKDTREE2_H
#define QKDTREE2_H

#include <QObject>
#include <QVector>
#include <QPointer>

#include "QADCModules_global.h"
#include "adcirc_errors.h"
#include "kdtree2.hpp"

class QADCMODULESSHARED_EXPORT qKdtree2 : public QObject
{
    Q_OBJECT
public:
    explicit qKdtree2(QObject *parent = 0);

    ~qKdtree2();

    ///Variable holding the total number of points in the search tree
    int numDataPoints;

    int build(QVector<double> &x, QVector<double> &y);

    int findNearest(double x, double y, int &index);

    int findXNearest(double x, double y, int nn, QVector<int> &indicies);

private:
    kdtree2* tree;

};

#endif // QKDTREE2_H
