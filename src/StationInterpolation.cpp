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
#include "StationInterpolation.h"

#include "Constants.h"
#include "FPCompare.h"
#include "FileIO.h"
#include "Logging.h"
#include "ProgressBar.h"
#include "Projection.h"
#include "boost/format.hpp"

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
    this->m_options.createStationObject(2);
  } else {
    this->m_options.createStationObject(1);
  }

  Adcirc::Output::OutputFormat filetype = globalFile.filetype();
  Adcirc::Geometry::Mesh m = this->readMesh(filetype);
  Adcirc::CDate coldstart = this->getColdstartDate();
  this->allocateStationArrays();
  this->generateInterpolationWeights(m);

  size_t nsnap = this->m_options.endsnap() - this->m_options.startsnap() + 1;

  ProgressBar progress_bar(nsnap);
  progress_bar.begin();

  for (size_t i = this->m_options.startsnap(); i <= this->m_options.endsnap();
       ++i) {
    this->interpolateTimeSnapToStations(i, writeVector, coldstart, globalFile);
    progress_bar.tick();
    globalFile.clearAt(0);
  }
  progress_bar.end();

  this->reprojectStationOutput();
  this->m_options.stations()->write(this->m_options.outputfile());
}

Adcirc::Geometry::Mesh StationInterpolation::readMesh(
    const Adcirc::Output::OutputFormat &filetype) {
  Adcirc::Geometry::Mesh mesh;
  if (filetype == Adcirc::Output::OutputNetcdf3 ||
      filetype == Adcirc::Output::OutputNetcdf4) {
    if (this->m_options.readasciimesh()) {
      mesh.setFilename(this->m_options.mesh());
    } else {
      if (!this->m_options.mesh().empty()) {
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
    double x2 = 0.0;
    double y2 = 0.0;

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

    if (eidx == Adcirc::Geometry::Mesh::ELEMENT_NOT_FOUND) {
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
}

Adcirc::CDate StationInterpolation::getColdstartDate() {
  Adcirc::Output::Hmdf::HmdfFileType ft =
      Adcirc::Output::Hmdf::getFiletype(this->m_options.outputfile());
  if (ft == Adcirc::Output::Hmdf::HmdfImeds ||
      ft == Adcirc::Output::Hmdf::HmdfCsv ||
      ft == Adcirc::Output::Hmdf::HmdfNetCdf) {
    return Adcirc::Output::StationInterpolation::dateFromString(
        this->m_options.coldstart());
  }
  return Adcirc::CDate(1970, 1, 1, 0, 0, 0);
}

void StationInterpolation::interpolateTimeSnapToStations(
    const size_t snap, const bool writeVector, const Adcirc::CDate &coldstart,
    Adcirc::Output::ReadOutput &globalFile) {
  globalFile.read(snap - 1);
  auto adcircTime = globalFile.dataAt(0)->time();
  auto adcircIt = globalFile.dataAt(0)->iteration();
  Adcirc::CDate d = coldstart + adcircTime;
  Hmdf *stationData = this->m_options.stations();

  for (size_t j = 0; j < stationData->nstations(); ++j) {
    if (this->m_weights[j].found) {
      if (writeVector) {
        stationData->station(j)->setNext(
            d, Adcirc::Output::StationInterpolation::interpVector(
                   globalFile, this->m_weights[j]));
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
    auto file_position = stationData->station(j)->numSnaps() - 1;
    stationData->station(j)->setAdcircTime(file_position, adcircTime);
    stationData->station(j)->setAdcircIteration(file_position, adcircIt);
  }
}

double StationInterpolation::interpScalar(Adcirc::Output::ReadOutput &data,
                                          Weight &w,
                                          const double positive_direction) {
  if (this->m_options.angle()) {
    if (data.metadata()->isVector()) {
      adcircmodules_throw_exception(
          "Vector data supplied when a scalar angle was expected");
    }
    return Adcirc::Output::StationInterpolation::interpAngle(data, w);
  } else if (data.metadata()->isVector()) {
    return this->interpScalarFromVector(data, w, positive_direction);
  } else {
    return Adcirc::Output::StationInterpolation::interpolateDryValues(
        data.dataAt(0)->z(w.node_index[0]), w.weight[0],
        data.dataAt(0)->z(w.node_index[1]), w.weight[1],
        data.dataAt(0)->z(w.node_index[2]), w.weight[2], data.defaultValue());
  }
}

double StationInterpolation::interpAngle(Adcirc::Output::ReadOutput &data,
                                         Weight &w) {
  using namespace Adcirc::FpCompare;
  std::array<double, 3> vx{0, 0, 0};
  std::array<double, 3> vy{0, 0, 0};
  for (size_t i = 0; i < 3; ++i) {
    auto v = data.dataAt(0)->z(w.node_index[i]);
    if (equalTo(v, data.defaultValue())) {
      vx[i] = data.defaultValue();
      vy[i] = data.defaultValue();
    } else {
      v *= Constants::deg2rad();
      vx[i] = std::cos(v);
      vy[i] = std::sin(v);
    }
  }
  auto vvx = StationInterpolation::interpolateDryValues(
      vx[0], w.weight[0], vx[1], w.weight[1], vx[2], w.weight[2],
      data.defaultValue());
  auto vvy = StationInterpolation::interpolateDryValues(
      vy[0], w.weight[0], vy[1], w.weight[1], vy[2], w.weight[2],
      data.defaultValue());
  if (equalTo(vvx, data.defaultValue()) || equalTo(vvy, data.defaultValue())) {
    return data.defaultValue();
  } else {
    auto angle = std::atan2(vvy, vvx) * Constants::rad2deg();
    return angle < 0.0 ? angle += 360.0 : angle;
  }
}

std::tuple<double, double> StationInterpolation::interpVector(
    Adcirc::Output::ReadOutput &data, Weight &w) {
  std::array<double, 3> vx{0, 0, 0};
  std::array<double, 3> vy{0, 0, 0};
  for (auto i = 0; i < 3; ++i) {
    vx[i] = data.dataAt(0)->u(w.node_index[i]);
    vy[i] = data.dataAt(0)->v(w.node_index[i]);
  }
  double vxx = StationInterpolation::interpolateDryValues(
      vx[0], w.weight[0], vx[1], w.weight[1], vx[2], w.weight[2],
      data.defaultValue());
  double vyy = StationInterpolation::interpolateDryValues(
      vy[0], w.weight[0], vy[1], w.weight[1], vy[2], w.weight[2],
      data.defaultValue());
  return std::make_tuple(vxx, vyy);
}

double StationInterpolation::interpScalarFromVectorWithFlowDirection(
    Adcirc::Output::ReadOutput &data, Weight &w,
    const double positive_direction) {
  using namespace Adcirc::FpCompare;
  double vx, vy;
  std::tie(vx, vy) = StationInterpolation::interpVector(data, w);
  if (equalTo(vx, data.defaultValue()) || equalTo(vy, data.defaultValue())) {
    return data.defaultValue();
  }
  double magnitude = std::sqrt(std::pow(vx, 2.0) + std::pow(vy, 2.0));
  double direction =
      std::atan2(vy, vx) * Adcirc::Constants::rad2deg() - positive_direction;

  if (direction < -180.0) {
    direction += 360.0;
  } else if (direction > 180.0) {
    direction -= 360.0;
  }

  return direction < 90.0 && direction > -90.0 ? magnitude : -magnitude;
}

double StationInterpolation::interpScalarFromVectorWithoutFlowDirection(
    Adcirc::Output::ReadOutput &data, Weight &w) {
  return StationInterpolation::interpolateDryValues(
      data.dataAt(0)->magnitude(w.node_index[0]), w.weight[0],
      data.dataAt(0)->magnitude(w.node_index[1]), w.weight[1],
      data.dataAt(0)->magnitude(w.node_index[2]), w.weight[2],
      data.defaultValue());
}

double StationInterpolation::interpDirectionFromVector(
    Adcirc::Output::ReadOutput &data, Weight &w) {
  using namespace Adcirc::FpCompare;
  double vx, vy;
  std::tie(vx, vy) = StationInterpolation::interpVector(data, w);
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
    return StationInterpolation::interpScalarFromVectorWithoutFlowDirection(
        data, w);
  } else if (this->m_options.magnitude() &&
             !equalTo(positive_direction, -9999.0)) {
    return StationInterpolation::interpScalarFromVectorWithFlowDirection(
        data, w, positive_direction);
  } else if (this->m_options.direction()) {
    return StationInterpolation::interpDirectionFromVector(data, w);
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
