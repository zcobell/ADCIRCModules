//------------------------------GPL---------------------------------------//
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
//------------------------------------------------------------------------//
#include "qproj4.h"
#include "point.h"
#include "proj.h"

using namespace std;

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for the proj4 wrapper class
 *
 * @param[in] *parent reference to QObject. Enables automatic memory management
 *to avoid memory leaks
 *
 * Constructs an object used to convert coordinates. The initialization function
 *is run at startup which parses the included EPSG file (:/rsc/epsg) to
 *determine the parameters to pass to the Proj4 API
 *
 **/
//-----------------------------------------------------------------------------------------//
QProj4::QProj4() { this->_initialize(); }
//-----------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------//
// Function to execute a coordinate system transformation using Proj4
//-----------------------------------------------------------------------------------------//
/** \brief Function to execute a coordinate system transformation using Proj4
 *
 * @param[in]  inputEPSG  EPSG that coordinates are currently in
 * @param[in]  outputEPSG EPSG that the coordinates will be converted to
 * @param[in]  input      Point object containing the location to be converted
 * @param[out] output     Point object containing the converted coordiantes
 * @param[out] isLatLon   Bool that determine if the coordinates are lat/lon or
 *otherwise
 *
 * Function to execute a coordinate system transformation using Proj4
 *
 **/
//-----------------------------------------------------------------------------------------//
int QProj4::transform(int inputEPSG, int outputEPSG, Point &input,
                      Point &output, bool &isLatLon) {
  if (this->m_epsgMapping.find(inputEPSG) == this->m_epsgMapping.end()) {
    return NoSuchProjection;
  }

  if (this->m_epsgMapping.find(outputEPSG) == this->m_epsgMapping.end()) {
    return NoSuchProjection;
  }

  string p1 = this->m_epsgMapping[inputEPSG];
  string p2 = this->m_epsgMapping[outputEPSG];

  PJ *pj1 = proj_create(PJ_DEFAULT_CTX, p1.c_str());
  if (pj1 == 0) {
    return Proj4InternalError;
  }

  PJ *pj2 = proj_create(PJ_DEFAULT_CTX, p2.c_str());
  if (pj2 == 0) {
    proj_destroy(pj1);
    return Proj4InternalError;
  }

  PJ_COORD c, o;

  if (proj_angular_input(pj1, PJ_FWD)) {
    c.lp.lam = proj_torad(input.x());
    c.lp.phi = proj_torad(input.y());
  } else {
    c.xy.x = input.x();
    c.xy.y = input.y();
  }

  o = proj_trans(pj2, PJ_FWD, proj_trans(pj1, PJ_INV, c));

  if (proj_angular_output(pj2, PJ_FWD)) {
    output.setX(proj_todeg(o.lp.lam));
    output.setY(proj_todeg(o.lp.phi));
    isLatLon = true;
  } else {
    output.setX(o.xy.x);
    output.setY(o.xy.y);
    isLatLon = false;
  }

  proj_destroy(pj1);
  proj_destroy(pj2);
  return NoError;
}
//-----------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------//
// Function to execute a coordinate system transformation using Proj4
//-----------------------------------------------------------------------------------------//
/** \brief Function to execute a coordinate system transformation using Proj4
 *
 * @param[in]  inputEPSG  EPSG that coordinates are currently in
 * @param[in]  outputEPSG EPSG that the coordinates will be converted to
 * @param[in]  input      vector of Point objects containing the locations
 *                        to be converted
 * @param[out] output     vector of Point objects containing the converted
 *                        coordiantes
 * @param[out] isLatLon   Bool that determine if the coordinates are lat/lon or
 *                        otherwise
 *
 * Function to execute a coordinate system transformation using Proj4
 *
 **/
//-----------------------------------------------------------------------------------------//
int QProj4::transform(int inputEPSG, int outputEPSG, vector<Point> &input,
                      vector<Point> &output, bool &isLatLon) {
  if (this->m_epsgMapping.find(inputEPSG) == this->m_epsgMapping.end()) {
    return NoSuchProjection;
  }

  if (this->m_epsgMapping.find(outputEPSG) == this->m_epsgMapping.end()) {
    return NoSuchProjection;
  }

  string p1 = this->m_epsgMapping[inputEPSG];
  string p2 = this->m_epsgMapping[outputEPSG];

  PJ *pj1 = proj_create(PJ_DEFAULT_CTX, p1.c_str());
  if (pj1 == 0) {
    return Proj4InternalError;
  }

  PJ *pj2 = proj_create(PJ_DEFAULT_CTX, p2.c_str());
  if (pj2 == 0) {
    proj_destroy(pj1);
    return Proj4InternalError;
  }

  for (size_t i = 0; i < input.size(); i++) {
    PJ_COORD c, o;

    if (proj_angular_input(pj1, PJ_FWD)) {
      c.lp.lam = proj_torad(input[i].x());
      c.lp.phi = proj_torad(input[i].y());
    } else {
      c.xy.x = input[i].x();
      c.xy.y = input[i].y();
    }

    o = proj_trans(pj2, PJ_FWD, proj_trans(pj1, PJ_INV, c));

    if (proj_angular_output(pj2, PJ_FWD)) {
      output[i].setX(proj_todeg(o.lp.lam));
      output[i].setY(proj_todeg(o.lp.phi));
      isLatLon = true;
    } else {
      output[i].setX(o.xy.x);
      output[i].setY(o.xy.y);
      isLatLon = false;
    }
  }

  proj_destroy(pj1);
  proj_destroy(pj2);
  return NoError;
}
//-----------------------------------------------------------------------------------------//
