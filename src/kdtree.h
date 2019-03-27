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
#ifndef KDTREE_H
#define KDTREE_H

#include <cstddef>
#include <vector>

class Kdtree_impl;

class Kdtree {
 public:
  Kdtree();
  ~Kdtree();

  enum _errors { NoError, SizeMismatch };

  int build(std::vector<double> &x, std::vector<double> &y);
  size_t findNearest(double x, double y);
  std::vector<size_t> findXNearest(double x, double y, size_t n);
  bool initialized();

 private:
  Kdtree_impl *m_ptr;
};

#endif  // KDTREE_H
