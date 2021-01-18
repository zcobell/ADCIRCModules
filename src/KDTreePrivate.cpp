/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
//
// ADCIRCModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ADCIRCModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------*/
#include "KDTreePrivate.h"

#include "KDTree.h"

using namespace Adcirc::Private;

Adcirc::Kdtree::~Kdtree() = default;

KdtreePrivate::KdtreePrivate() : m_initialized(false) {}

bool KdtreePrivate::initialized() { return this->m_initialized; }

size_t KdtreePrivate::size() { return this->m_cloud.pts.size(); }

int KdtreePrivate::build(std::vector<double> &x, std::vector<double> &y) {
  if (x.size() != y.size()) return 1;
  this->m_cloud.pts.resize(x.size());
  for (size_t i = 0; i < x.size(); ++i) {
    this->m_cloud.pts[i].x = x[i];
    this->m_cloud.pts[i].y = y[i];
  }
  this->m_tree = std::make_unique<kd_tree_t>(
      2, this->m_cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10));
  this->m_tree->buildIndex();
  this->m_initialized = true;
  return 0;
}

size_t KdtreePrivate::findNearest(double x, double y) {
  size_t index;
  double out_dist_sqr;
  nanoflann::KNNResultSet<double> resultSet(1);
  resultSet.init(&index, &out_dist_sqr);
  const double query_pt[2] = {x, y};
  this->m_tree->findNeighbors(resultSet, &query_pt[0],
                              nanoflann::SearchParams(10));
  return index;
}

std::vector<size_t> KdtreePrivate::findXNearest(double x, double y, size_t n) {
  n = std::min(this->size(), n);
  std::vector<size_t> index(n);
  std::vector<double> out_dist_sqr(n);

  nanoflann::KNNResultSet<double> resultSet(n);
  resultSet.init(index.data(), out_dist_sqr.data());
  const double query_pt[2] = {x, y};

  this->m_tree->findNeighbors(resultSet, &query_pt[0],
                              nanoflann::SearchParams(10));
  return index;
}

std::vector<size_t> KdtreePrivate::findWithinRadius(double x, double y,
                                                    const double radius) {
  //...Square radius since distance metric is a square distance
  const double search_radius = std::pow(radius, 2.0);

  const double query_pt[2] = {x, y};

  std::vector<std::pair<size_t, double>> matches;
  nanoflann::SearchParams params;

  //...This is the default, but making it explicit for futureproofing
  params.sorted = true;

  //...Perform the search
  const size_t nmatches =
      this->m_tree->radiusSearch(query_pt, search_radius, matches, params);

  //...Save the indicies into a vector to return
  std::vector<size_t> outMatches;
  outMatches.reserve(nmatches);
  for (auto &match : matches) {
    outMatches.push_back(match.first);
  }
  return outMatches;
}
