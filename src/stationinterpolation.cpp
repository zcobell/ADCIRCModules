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
#include "stationinterpolation.h"

#include "boost/format.hpp"
#include "boost/progress.hpp"
#include "constants.h"
#include "fileio.h"
#include "fpcompare.h"
#include "logging.h"
#include "projection.h"

using namespace Adcirc::Output;

StationInterpolation::StationInterpolation(
    const StationInterpolationOptions &options)
    : m_options(options) {}

void StationInterpolation::run() {
  Adcirc::Output::ReadOutput globalFile(this->m_options.globalfile());
  globalFile.open();

  bool writeVector = false;
  if (globalFile.metadata()->isVector() && !this->m_options.magnitude() &&
      !this->m_options.direction()) {
    writeVector = true;
    this->m_options.stations()->setVector(true);
  }

  Adcirc::Output::OutputFormat filetype = globalFile.filetype();
  Adcirc::Geometry::Mesh m = this->readMesh(filetype);
  Adcirc::CDate coldstart = this->getColdstartDate();
  this->allocateStationArrays();
  this->generateInterpolationWeights(m);

  size_t nsnap = this->m_options.endsnap() - this->m_options.startsnap() + 1;

  boost::progress_display progress_bar(nsnap);

  for (size_t i = this->m_options.startsnap(); i <= this->m_options.endsnap();
       ++i) {
    this->interpolateTimeSnapToStations(i, writeVector, coldstart, globalFile);
    ++progress_bar;
    globalFile.clearAt(0);
  }

  this->reprojectStationOutput();
  this->m_options.stations()->write(this->m_options.outputfile());

  return;
}

Adcirc::Geometry::Mesh StationInterpolation::readMesh(
    const Adcirc::Output::OutputFormat &filetype) {
  Adcirc::Geometry::Mesh mesh;
  if (filetype == Adcirc::Output::OutputNetcdf3 ||
      filetype == Adcirc::Output::OutputNetcdf4) {
    if (this->m_options.readasciimesh()) {
      mesh.setFilename(this->m_options.mesh());
    } else {
      if (this->m_options.mesh() != std::string()) {
        Adcirc::Logging::warning(
            "Ignoring provided mesh file in favor of netCDF data",
            "[WARNING]: ");
      }
      mesh.setFilename(this->m_options.globalfile());
    }
  } else {
    mesh.setFilename(this->m_options.mesh());
  }
  mesh.read();
  return mesh;
}

void StationInterpolation::generateInterpolationWeights(
    Adcirc::Geometry::Mesh &m) {
  m.buildElementalSearchTree();
  size_t nFound = 0;
  Hmdf *stn = this->m_options.stations();

  this->m_weights.resize(stn->nstations());

  for (size_t i = 0; i < stn->nstations(); ++i) {
    std::vector<double> wt;
    double x1 = stn->station(i)->longitude();
    double y1 = stn->station(i)->latitude();
    double x2 = 0;
    double y2 = 0;

    if (this->m_options.epsgStation() != this->m_options.epsgGlobal()) {
      bool isLatLon;
      Adcirc::Projection::transform(this->m_options.epsgStation(),
                                    this->m_options.epsgGlobal(), x1, y1, x2,
                                    y2, isLatLon);
    } else {
      x2 = x1;
      y2 = y1;
    }

    size_t eidx = m.findElement(x2, y2, wt);

    if (eidx == m.ELEMENT_NOT_FOUND) {
      this->m_weights[i].found = false;
    } else {
      nFound++;
      for (size_t j = 0; j < 3; ++j) {
        this->m_weights[i].found = true;
        this->m_weights[i].node_index[j] =
            m.nodeIndexById(m.element(eidx)->node(j)->id());
        this->m_weights[i].weight[j] = wt[j];
      }
    }
  }

  Adcirc::Logging::log(
      boost::str(boost::format("%i of %i stations found inside the mesh") %
                 nFound % stn->nstations()),
      "[INFO]: ");
  if (nFound == 0) {
    Adcirc::Logging::logError(
        "No stations found within mesh for interpolation.", "[ERROR]: ");
    exit(1);
  }
  return;
}

void StationInterpolation::allocateStationArrays() {
  for (size_t i = 0; i < this->m_options.stations()->nstations(); ++i) {
    this->m_options.station(i)->reserve(this->m_options.endsnap() -
                                        this->m_options.startsnap());
  }
}

void StationInterpolation::reprojectStationOutput() {
  if (this->m_options.epsgStation() != this->m_options.epsgOutput()) {
    Hmdf *output = this->m_options.stations();
    for (size_t i = 0; i < output->nstations(); ++i) {
      double x1 = output->station(i)->longitude();
      double y1 = output->station(i)->latitude();
      double x2 = 0;
      double y2 = 0;
      bool isLatLon;
      Adcirc::Projection::transform(this->m_options.epsgStation(),
                                    this->m_options.epsgOutput(), x1, y1, x2,
                                    y2, isLatLon);
      output->station(i)->setLongitude(x2);
      output->station(i)->setLatitude(y2);
    }
  }
  return;
}

Adcirc::CDate StationInterpolation::getColdstartDate() {
  Adcirc::Output::Hmdf::HmdfFileType ft =
      Adcirc::Output::Hmdf::getFiletype(this->m_options.outputfile());
  if (ft == Adcirc::Output::Hmdf::HmdfImeds ||
      ft == Adcirc::Output::Hmdf::HmdfCsv ||
      ft == Adcirc::Output::Hmdf::HmdfNetCdf) {
    return this->dateFromString(this->m_options.coldstart());
  }
  return Adcirc::CDate(1970, 1, 1, 0, 0, 0);
}

void StationInterpolation::interpolateTimeSnapToStations(
    const size_t snap, const bool writeVector, const Adcirc::CDate &coldstart,
    Adcirc::Output::ReadOutput &globalFile) {
  globalFile.read(snap - 1);
  Adcirc::CDate d = coldstart + globalFile.dataAt(0)->time();
  Hmdf *stationData = this->m_options.stations();

  for (size_t j = 0; j < stationData->nstations(); ++j) {
    if (this->m_weights[j].found) {
      if (writeVector) {
        stationData->station(j)->setNext(
            d, this->interpVector(globalFile, this->m_weights[j]));
      } else {
        if (this->m_options.hasPositiveDirection()) {
          stationData->station(j)->setNext(
              d, this->interpScalar(
                     globalFile, this->m_weights[j],
                     this->m_options.station(j)->positiveDirection()));
        } else {
          stationData->station(j)->setNext(
              d, this->interpScalar(globalFile, this->m_weights[j]));
        }
      }
    } else {
      if (writeVector) {
        stationData->station(j)->setNext(d, globalFile.defaultValue(),
                                         globalFile.defaultValue());
      } else {
        stationData->station(j)->setNext(d, globalFile.defaultValue());
      }
    }
  }
  return;
}

double StationInterpolation::interpScalar(Adcirc::Output::ReadOutput &data,
                                          Weight &w,
                                          const double positive_direction) {
  if (data.metadata()->isVector()) {
    return this->interpScalarFromVector(data, w, positive_direction);
  } else {
    double v1 = data.dataAt(0)->z(w.node_index[0]);
    double v2 = data.dataAt(0)->z(w.node_index[1]);
    double v3 = data.dataAt(0)->z(w.node_index[2]);
    return this->interpolateDryValues(v1, w.weight[0], v2, w.weight[1], v3,
                                      w.weight[2], data.defaultValue());
  }
}

std::tuple<double, double> StationInterpolation::interpVector(
    Adcirc::Output::ReadOutput &data, Weight &w) {
  size_t n1 = w.node_index[0];
  size_t n2 = w.node_index[1];
  size_t n3 = w.node_index[2];
  double w1 = w.weight[0];
  double w2 = w.weight[1];
  double w3 = w.weight[2];
  double v1x = data.dataAt(0)->u(n1);
  double v1y = data.dataAt(0)->v(n1);
  double v2x = data.dataAt(0)->u(n2);
  double v2y = data.dataAt(0)->v(n2);
  double v3x = data.dataAt(0)->u(n3);
  double v3y = data.dataAt(0)->v(n3);
  double vx = this->interpolateDryValues(v1x, w1, v2x, w2, v3x, w3,
                                         data.defaultValue());
  double vy = this->interpolateDryValues(v1y, w1, v2y, w2, v3y, w3,
                                         data.defaultValue());
  return std::make_tuple(vx, vy);
}

double StationInterpolation::interpScalarFromVectorWithFlowDirection(
    Adcirc::Output::ReadOutput &data, Weight &w,
    const double positive_direction) {
  using namespace Adcirc::FpCompare;
  size_t n1 = w.node_index[0];
  size_t n2 = w.node_index[1];
  size_t n3 = w.node_index[2];
  double w1 = w.weight[0];
  double w2 = w.weight[1];
  double w3 = w.weight[2];
  double v1x = data.dataAt(0)->u(n1);
  double v1y = data.dataAt(0)->v(n1);
  double v2x = data.dataAt(0)->u(n2);
  double v2y = data.dataAt(0)->v(n2);
  double v3x = data.dataAt(0)->u(n3);
  double v3y = data.dataAt(0)->v(n3);
  double vx = this->interpolateDryValues(v1x, w1, v2x, w2, v3x, w3,
                                         data.defaultValue());
  double vy = this->interpolateDryValues(v1y, w1, v2y, w2, v3y, w3,
                                         data.defaultValue());
  if (equalTo(vx, data.defaultValue()) || equalTo(vy, data.defaultValue())) {
    return data.defaultValue();
  }
  double magnitude = std::sqrt(std::pow(vx, 2.0) + std::pow(vy, 2.0));
  double direction =
      std::atan2(vy, vx) * Adcirc::Constants::rad2deg() - positive_direction;

  if (direction < -180.0)
    direction += 360.0;
  else if (direction > 180.0)
    direction -= 360.0;

  return direction < 90.0 && direction > -90.0 ? magnitude : -magnitude;
}

double StationInterpolation::interpScalarFromVectorWithoutFlowDirection(
    Adcirc::Output::ReadOutput &data, Weight &w) {
  double v1 = data.dataAt(0)->magnitude(w.node_index[0]);
  double v2 = data.dataAt(0)->magnitude(w.node_index[1]);
  double v3 = data.dataAt(0)->magnitude(w.node_index[2]);
  return this->interpolateDryValues(v1, w.weight[0], v2, w.weight[1], v3,
                                    w.weight[2], data.defaultValue());
}

double StationInterpolation::interpDirectionFromVector(
    Adcirc::Output::ReadOutput &data, Weight &w) {
  using namespace Adcirc::FpCompare;
  size_t n1 = w.node_index[0];
  size_t n2 = w.node_index[1];
  size_t n3 = w.node_index[2];
  double w1 = w.weight[0];
  double w2 = w.weight[1];
  double w3 = w.weight[2];
  double v1x = data.dataAt(0)->u(n1);
  double v1y = data.dataAt(0)->v(n1);
  double v2x = data.dataAt(0)->u(n2);
  double v2y = data.dataAt(0)->v(n2);
  double v3x = data.dataAt(0)->u(n3);
  double v3y = data.dataAt(0)->v(n3);
  double vx = this->interpolateDryValues(v1x, w1, v2x, w2, v3x, w3,
                                         data.defaultValue());
  double vy = this->interpolateDryValues(v1y, w1, v2y, w2, v3y, w3,
                                         data.defaultValue());
  if (equalTo(vx, data.defaultValue()) || equalTo(vy, data.defaultValue())) {
    return data.defaultValue();
  } else {
    return std::atan2(vy, vx) * Adcirc::Constants::rad2deg();
  }
}

double StationInterpolation::interpScalarFromVector(
    Adcirc::Output::ReadOutput &data, Weight &w,
    const double positive_direction) {
  using namespace Adcirc::FpCompare;
  if (this->m_options.magnitude() && equalTo(positive_direction, -9999.0)) {
    return this->interpScalarFromVectorWithoutFlowDirection(data, w);
  } else if (this->m_options.magnitude() &&
             !equalTo(positive_direction, -9999.0)) {
    return this->interpScalarFromVectorWithFlowDirection(data, w,
                                                         positive_direction);
  } else if (this->m_options.direction()) {
    return this->interpDirectionFromVector(data, w);
  } else {
    adcircmodules_throw_exception(
        "Cannot write vector data. Select --magnitude or --direction");
    return data.defaultValue();
  }
}

double StationInterpolation::interpolateDryValues(double v1, double w1,
                                                  double v2, double w2,
                                                  double v3, double w3,
                                                  double defaultVal) {
  using namespace Adcirc::FpCompare;

  if (equalTo(v1, defaultVal) || equalTo(v2, defaultVal) ||
      equalTo(v3, defaultVal)) {
    if (equalTo(v1, defaultVal) && !equalTo(v2, defaultVal) &&
        !equalTo(v3, defaultVal)) {
      double f = 1.0 / (w2 + w3);
      w1 = 0.0;
      w2 = w2 * f;
      w3 = w3 * f;
    } else if (!equalTo(v1, defaultVal) && equalTo(v2, defaultVal) &&
               !equalTo(v3, defaultVal)) {
      double f = 1.0 / (w1 + w3);
      w1 = w1 * f;
      w2 = 0.0;
      w3 = w3 * f;
    } else if (!equalTo(v1, defaultVal) && !equalTo(v2, defaultVal) &&
               equalTo(v3, defaultVal)) {
      double f = 1.0 / (w1 + w2);
      w1 = w1 * f;
      w2 = w2 * f;
      w3 = 0.0;
    } else if (equalTo(v1, defaultVal) && equalTo(v2, defaultVal) &&
               !equalTo(v3, defaultVal)) {
      return v3;
    } else if (equalTo(v1, defaultVal) && !equalTo(v2, defaultVal) &&
               equalTo(v3, defaultVal)) {
      return v2;
    } else if (!equalTo(v1, defaultVal) && equalTo(v2, defaultVal) &&
               equalTo(v3, defaultVal)) {
      return v1;
    } else {
      return defaultVal;
    }
  }
  return v1 * w1 + v2 * w2 + v3 * w3;
}

Adcirc::CDate StationInterpolation::dateFromString(
    const std::string &dateString) {
  int year = stoi(dateString.substr(0, 4));
  int month = stoi(dateString.substr(4, 2));
  int day = stoi(dateString.substr(6, 2));
  int hour = stoi(dateString.substr(8, 2));
  int minute = stoi(dateString.substr(10, 2));
  int second = stoi(dateString.substr(12, 2));
  return Adcirc::CDate(year, month, day, hour, minute, second);
}
