#ifndef ADCIRCMESH_H
#define ADCIRCMESH_H

#include "adcircboundary.h"
#include "adcircelement.h"
#include "adcircenum.h"
#include "adcircmodules_global.h"
#include "adcircnode.h"
#include <QFile>
#include <QHash>
#include <QObject>
#include <QVector>

class ADCIRCMODULESSHARED_EXPORT AdcircMesh : public QObject {
  Q_OBJECT
public:
  explicit AdcircMesh(QObject *parent = nullptr);
  explicit AdcircMesh(QString filename, QObject *parent = nullptr);

  ~AdcircMesh();

  int read(QString filename);
  int read();

  QString filename() const;
  void setFilename(const QString &filename);

  QString meshHeaderString() const;
  void setMeshHeaderString(const QString &meshHeaderString);

  int numNodes() const;
  void setNumNodes(int numNodes);

  int numElements() const;
  void setNumElements(int numElements);

  int numOpenBoundaries() const;
  void setNumOpenBoundaries(int numOpenBoundaries);

  int numLandBoundaries() const;
  void setNumLandBoundaries(int numLandBoundaries);

  int totalOpenBoundaryNodes() const;
  void setTotalOpenBoundaryNodes(int totalOpenBoundaryNodes);

  int totalLandBoundaryNodes() const;
  void setTotalLandBoundaryNodes(int totalLandBoundaryNodes);

private:
  int _readNodes(QFile &fid);
  int _readElements(QFile &fid);
  int _readOpenBoundaries(QFile &fid);
  int _readLandBoundaries(QFile &fid);

  QString _filename;
  QString _meshHeaderString;
  QVector<AdcircNode *> _nodes;
  QVector<AdcircElement *> _elements;
  QVector<AdcircBoundary *> _openBoundaries;
  QVector<AdcircBoundary *> _landBoundaries;
  QHash<int, int> _nodeLookup;
  int _numNodes;
  int _numElements;
  int _numOpenBoundaries;
  int _numLandBoundaries;
  int _totalOpenBoundaryNodes;
  int _totalLandBoundaryNodes;
};

#endif // ADCIRCMESH_H
