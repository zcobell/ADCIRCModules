/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2018 Zachary Cobell
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
#include "kdtree_impl.h"

Kdtree_impl::Kdtree_impl() : m_initialized(false) {}

bool Kdtree_impl::initialized() { return this->m_initialized; }

int Kdtree_impl::build(std::vector<double> &x, std::vector<double> &y) {
  if (x.size() != y.size()) return 1;
  this->m_cloud.pts.resize(x.size());
  for (size_t i = 0; i < x.size(); ++i) {
    this->m_cloud.pts[i].x = x[i];
    this->m_cloud.pts[i].y = y[i];
  }
  this->m_tree = std::unique_ptr<kd_tree_t>(
      new kd_tree_t(2, m_cloud, nanoflann::KDTreeSingleIndexAdaptorParams(10)));
  this->m_tree->buildIndex();
  this->m_initialized = true;
  return 0;
}

size_t Kdtree_impl::findNearest(double x, double y) {
  size_t index;
  double out_dist_sqr;
  nanoflann::KNNResultSet<double> resultSet(1);
  resultSet.init(&index, &out_dist_sqr);
  double query_pt[2] = {x, y};
  this->m_tree->findNeighbors(resultSet, query_pt, nanoflann::SearchParams(10));
  return index;
}

std::vector<size_t> Kdtree_impl::findXNearest(double x, double y, size_t n) {
  size_t *index = new size_t[n];
  double *out_dist_sqr = new double[n];
  nanoflann::KNNResultSet<double> resultSet(n);
  resultSet.init(index, out_dist_sqr);
  double query_pt[2] = {x, y};
  this->m_tree->findNeighbors(resultSet, query_pt, nanoflann::SearchParams(10));
  std::vector<size_t> result(index, index + n);
  delete[] index;
  delete[] out_dist_sqr;
  return result;
}
