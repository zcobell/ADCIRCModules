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
#ifndef LTEA_H
#define LTEA_H

#include <cmath>
#include <list>
#include "mesh.h"
#include "elementtable.h"

class Ltea {
 public:
  Ltea(Adcirc::Geometry::Mesh *mesh);

 private:
  //...Constants
  const double s = 1.0;
  const double sw = 3.0;
  const double initiala = 2.0e+07;
  const double influence = 3.5;
  const int nlistsize = 10000;
  const int elistsize = nlistsize;
  const int pelsize = sqrt(nlistsize);
  const int pnlsize = sqrt(nlistsize);
  const int layerlocsize = pnlsize;
  const double baddata = -999999.0;
  const int mesh = 14;
  const int num = 1;
  const int loc = 2;
  const int layer = 3;
  const int in = 1;
  const int out = 0;
  const int unkn = -1;
  const int boundary = 1;
  const int yes = 1;
  const int no = 0;
  const int outfile = 60;
  const double c0 = 1.0;
  const double c1 = 0.0;
  const double c2 = 3.43;
  const double ihat[2] = {0.0, 1.0};
  const double vect[2] = {0.5, 0.866025403784439};
  const double vect2[2] = {0.866025403784439, 0.5};

  //...Internal variables
  Adcirc::Geometry::Mesh *m_mesh;

  Adcirc::Geometry::ElementTable m_inverseConnectivity;
};

#endif  // LTEA_H
