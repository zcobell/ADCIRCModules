/*-------------------------------GPL-------------------------------------//
//
// adcircmodules - A library for working with ADCIRC models
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
#include "qproj4.h"
#include "proj_api.h"

//-----------------------------------------------------------------------------------------//
// Initializer
//-----------------------------------------------------------------------------------------//
/** \brief Constructor for the proj4 wrapper class
 *
 * @param[in] *parent reference to QObject. Enables automatic memory management
 *to avoid memory leaks
 *
 * Constructs an object used to convert coordinates. The initialization function
 *is run at
 * startup which parses the included EPSG file (:/rsc/epsg) to determine the
 *parameters to pass
 * to the Proj4 API
 *
 **/
//-----------------------------------------------------------------------------------------//
QProj4::QProj4() {this->_initialize();}
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
  projPJ inputPJ, outputPJ;
  double x, y, z;
  int ierr;

  z = 0.0;

  if (this->m_epsgMapping.find(inputEPSG) == this->m_epsgMapping.end())
    return NoSuchProjection;

  if (this->m_epsgMapping.find(outputEPSG) == this->m_epsgMapping.end())
    return NoSuchProjection;

  std::string currentInitialization = this->m_epsgMapping[inputEPSG];
  std::string outputInitialization = this->m_epsgMapping[outputEPSG];

  if (!(inputPJ = pj_init_plus(currentInitialization.c_str())))
    return Proj4InternalError;

  if (!(outputPJ = pj_init_plus(outputInitialization.c_str())))
    return Proj4InternalError;

  if (pj_is_latlong(inputPJ)) {
    x = input.x() * DEG_TO_RAD;
    y = input.y() * DEG_TO_RAD;
  } else {
    x = input.x();
    y = input.y();
  }

  ierr = pj_transform(inputPJ, outputPJ, 1, 1, &x, &y, &z);

  if (ierr != 0)
    return Proj4InternalError;

  if (pj_is_latlong(outputPJ)) {
    output.setX(x * RAD_TO_DEG);
    output.setY(y * RAD_TO_DEG);
    isLatLon = true;
  } else {
    output.setX(x);
    output.setY(y);
    isLatLon = false;
  }

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
 * @param[in]  input      std::vector of Point objects containing the locations
 *to
 *be converted
 * @param[out] output     std::vector of Point objects containing the converted
 *coordiantes
 * @param[out] isLatLon   Bool that determine if the coordinates are lat/lon or
 *otherwise
 *
 * Function to execute a coordinate system transformation using Proj4
 *
 **/
//-----------------------------------------------------------------------------------------//
int QProj4::transform(int inputEPSG, int outputEPSG, std::vector<Point> &input,
                      std::vector<Point> &output, bool &isLatLon) {
  projPJ inputPJ, outputPJ;
  std::vector<double> x, y, z;
  int i, ierr;

  if (this->m_epsgMapping.find(inputEPSG) == this->m_epsgMapping.end())
    return NoSuchProjection;

  if (this->m_epsgMapping.find(outputEPSG) == this->m_epsgMapping.end())
    return NoSuchProjection;

  std::string currentInitialization = this->m_epsgMapping[inputEPSG];
  std::string outputInitialization = this->m_epsgMapping[outputEPSG];

  if (!(inputPJ = pj_init_plus(currentInitialization.c_str())))
    return Proj4InternalError;

  if (!(outputPJ = pj_init_plus(outputInitialization.c_str())))
    return Proj4InternalError;

  x.resize(input.size());
  y.resize(input.size());
  z.resize(input.size());
  std::fill(z.begin(), z.end(), 0.0);
  output.resize(input.size());

  for (i = 0; i < input.size(); i++) {
    if (pj_is_latlong(inputPJ)) {
      x[i] = input[i].x() * DEG_TO_RAD;
      y[i] = input[i].y() * DEG_TO_RAD;
    } else {
      x[i] = input[i].x();
      y[i] = input[i].y();
    }

    ierr = pj_transform(inputPJ, outputPJ, 1, 1, &x[i], &y[i], &z[i]);

    if (ierr != 0)
      return Proj4InternalError;

    if (pj_is_latlong(outputPJ)) {
      output[i].setX(x[i] * RAD_TO_DEG);
      output[i].setY(y[i] * RAD_TO_DEG);
    } else {
      output[i].setX(x[i]);
      output[i].setY(y[i]);
    }
  }

  if (pj_is_latlong(outputPJ))
    isLatLon = true;
  else
    isLatLon = false;

  return NoError;
}
//-----------------------------------------------------------------------------------------//

