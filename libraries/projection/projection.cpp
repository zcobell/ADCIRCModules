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
#include "projection.h"
#include <cassert>
#include <cmath>
#include <limits>
#include "constants.h"
#include "point.h"
#include "proj.h"

/** \brief Constructor for the proj4 wrapper class
 *
 * Constructs an object used to convert coordinates. The initialization function
 *is run at startup which parses the included EPSG file (:/rsc/epsg) to
 *determine the parameters to pass to the Proj4 API
 *
 **/
Projection::Projection() { this->_initialize(); }

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
int Projection::transform(int inputEPSG, int outputEPSG, Point &input,
                          Point &output, bool &isLatLon) {
  std::vector<Point> in, out;
  in.push_back(input);
  int ierr = this->transform(inputEPSG, outputEPSG, in, out, isLatLon);
  if (ierr != Projection::NoError) return ierr;
  output = out.at(0);
  return ierr;
}

int Projection::transform(int inputEPSG, int outputEPSG, double x, double y,
                          double &outx, double &outy, bool &isLatLon) {
  Point in(x, y), out;
  int ierr = this->transform(inputEPSG, outputEPSG, in, out, isLatLon);
  if (ierr != Projection::NoError) return ierr;
  outx = out.x();
  outy = out.y();
  return ierr;
}

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
int Projection::transform(int inputEPSG, int outputEPSG,
                          std::vector<Point> &input, std::vector<Point> &output,
                          bool &isLatLon) {
  assert(input.size() > 0);
  if (input.size() <= 0) return Projection::NoData;

  if (this->m_epsgMapping.find(inputEPSG) == this->m_epsgMapping.end()) {
    return NoSuchProjection;
  }

  if (this->m_epsgMapping.find(outputEPSG) == this->m_epsgMapping.end()) {
    return NoSuchProjection;
  }

  std::string p1 = this->projInitializationString(inputEPSG);
  std::string p2 = this->projInitializationString(outputEPSG);

  PJ *pj1 = proj_create(PJ_DEFAULT_CTX, p1.c_str());
  if (pj1 == nullptr) {
    return Proj4InternalError;
  }

  PJ *pj2 = proj_create(PJ_DEFAULT_CTX, p2.c_str());
  if (pj2 == nullptr) {
    proj_destroy(pj1);
    return Proj4InternalError;
  }

  output.resize(input.size());

  for (size_t i = 0; i < input.size(); ++i) {
    PJ_COORD c, o;

    if (proj_angular_input(pj1, PJ_INV)) {
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

int Projection::cpp(double lambda0, double phi0, double x, double y,
                    double &outx, double &outy) {
  Point i(x, y), o;
  int ierr = Projection::cpp(lambda0, phi0, i, o);
  if (ierr != Projection::NoError) return ierr;
  outx = o.x();
  outy = o.y();
  return Projection::NoError;
}

int Projection::cpp(double lambda0, double phi0, Point &input, Point &output) {
  std::vector<Point> in, out;
  in.push_back(input);
  int ierr = Projection::cpp(lambda0, phi0, in, out);
  if (ierr == Projection::NoError) output = out.at(0);
  return ierr;
}

int Projection::cpp(double lambda0, double phi0, std::vector<Point> &input,
                    std::vector<Point> &output) {
  assert(input.size() > 0);
  if (input.size() <= 0) return Projection::NoData;

  double slam0 = Constants::toRadians(lambda0);
  double sfea0 = Constants::toRadians(phi0);
  double r = Constants::radiusEarth(phi0);
  output.reserve(input.size());
  for (auto &p : input) {
    double x = r * (Constants::toRadians(p.x()) - slam0) * cos(sfea0);
    double y = r * (Constants::toRadians(p.y()));
    output.push_back(Point(x, y));
  }
  return Projection::NoError;
}

int Projection::inverseCpp(double lambda0, double phi0, double x, double y,
                           double &outx, double &outy) {
  Point i(x, y), o;
  int ierr = Projection::inverseCpp(lambda0, phi0, i, o);
  if (ierr != Projection::NoError) return ierr;
  outx = o.x();
  outy = o.y();
  return Projection::NoError;
}

int Projection::inverseCpp(double lambda0, double phi0, Point &input,
                           Point &output) {
  std::vector<Point> in, out;
  in.push_back(input);
  int ierr = Projection::inverseCpp(lambda0, phi0, in, out);
  if (ierr == Projection::NoError) output = out.at(0);
  return ierr;
}

int Projection::inverseCpp(double lambda0, double phi0,
                           std::vector<Point> &input,
                           std::vector<Point> &output) {
  assert(input.size() > 0);
  if (input.size() <= 0) return Projection::NoData;

  double slam0 = Constants::toRadians(lambda0);
  double sfea0 = Constants::toRadians(phi0);
  double r = Constants::radiusEarth(phi0);
  output.reserve(input.size());
  for (auto &p : input) {
    double x = Constants::toDegrees(slam0 + p.x() / (r * cos(sfea0)));
    double y = Constants::toDegrees(p.y() / r);
    output.push_back(Point(x, y));
  }
  return Projection::NoError;
}

std::string Projection::projInitializationString(int epsg) {
  size_t i = this->position(epsg);
  if (i == std::numeric_limits<size_t>::max()) {
    return "No EPSG found";
  } else {
    return this->m_epsgInit[i];
  }
}

std::string Projection::description(int epsg) {
  size_t i = this->position(epsg);
  if (i == std::numeric_limits<size_t>::max()) {
    return "No EPSG found";
  } else {
    return this->m_epsgDescriptions[i];
  }
}

std::string Projection::projVersion() {
  return std::to_string(PROJ_VERSION_MAJOR) + "." +
         std::to_string(PROJ_VERSION_MINOR) + "." +
         std::to_string(PROJ_VERSION_PATCH);
}

size_t Projection::position(int epsg) {
  auto it = this->m_epsgMapping.find(epsg);
  if (it == this->m_epsgMapping.end()) {
    return std::numeric_limits<size_t>::max();
  } else {
    return this->m_epsgMapping[epsg];
  }
}
