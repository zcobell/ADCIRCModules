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
#include <string>

class QKdtree2;

class ADCIRCMODULESSHARED_EXPORT AdcircMesh : public QObject {
  Q_OBJECT
public:
  explicit AdcircMesh(QObject *parent = nullptr);
  explicit AdcircMesh(QString filename, QObject *parent = nullptr);
  explicit AdcircMesh(std::string filename, QObject *parent = nullptr);

  ~AdcircMesh();

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

  int projection();
  void defineProjection(int epsg, bool isLatLon);
  int reproject(int epsg);
  bool isLatLon();

  int toShapefile(QString outputFile);

  int buildNodalSearchTree();
  int buildElementalSearchTree();

  bool nodalSearchTreeInitialized();
  bool elementalSearchTreeInitialized();

  AdcircNode *node(int index);
  AdcircElement *element(int index);

  void resizeMesh(int numNodes, int numElements, int numOpenBoundaries,
                  int numLandBoundaries);

  void addNode(int index, AdcircNode *node);
  void deleteNode(int index);

  void addElement(int index, AdcircElement *element);
  void deleteElement(int index);

private:
  int _readNodes(QFile &fid);
  int _readElements(QFile &fid);
  int _readOpenBoundaries(QFile &fid);
  int _readLandBoundaries(QFile &fid);

  QString m_filename;
  QString m_meshHeaderString;
  QVector<AdcircNode *> m_nodes;
  QVector<AdcircElement *> m_elements;
  QVector<AdcircBoundary *> m_openBoundaries;
  QVector<AdcircBoundary *> m_landBoundaries;
  QHash<int, int> m_nodeLookup;
  int m_numNodes;
  int m_numElements;
  int m_numOpenBoundaries;
  int m_numLandBoundaries;
  int m_totalOpenBoundaryNodes;
  int m_totalLandBoundaryNodes;
  int m_epsg;
  bool m_isLatLon;
  QKdtree2 *m_nodalSearchTree;
  QKdtree2 *m_elementalSearchTree;
};

#endif // ADCIRCMESH_H
