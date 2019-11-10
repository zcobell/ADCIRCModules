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
#include "interpolate.h"
#include "boost/format.hpp"
#include "constants.h"
#include "ezproj.h"
#include "fpcompare.h"
#include "logging.h"

Interpolate::Interpolate(const InputOptions &options)
    : m_inputOptions(options) {}

void Interpolate::run() {
  Adcirc::Output::ReadOutput globalFile(this->m_inputOptions.globalfile);
  globalFile.open();

  bool writeVector = false;
  if (globalFile.metadata()->isVector() && !this->m_inputOptions.magnitude &&
      !this->m_inputOptions.direction) {
    writeVector = true;
  }

  Adcirc::Output::Hmdf stationData = this->readStationLocations(writeVector);
  this->validatePositiveFlowDirections(stationData);
  Adcirc::Output::OutputFormat filetype = globalFile.filetype();
  Adcirc::Geometry::Mesh m = this->readMesh(filetype);
  Date coldstart = this->getColdstartDate();
  this->allocateStationArrays(stationData);
  this->generateInterpolationWeights(m, stationData);

  size_t nsnap =
      this->m_inputOptions.endsnap - this->m_inputOptions.startsnap + 1;

  bool hasDirection =
      this->m_inputOptions.positive_direction != std::vector<double>();

  for (size_t i = this->m_inputOptions.startsnap;
       i <= this->m_inputOptions.endsnap; ++i) {
    this->interpolateTimeSnapToStations(i, writeVector, hasDirection, coldstart,
                                        globalFile, stationData);
    this->logProgress(nsnap, i);
    globalFile.clearAt(0);
  }

  this->reprojectStationOutput(stationData);
  stationData.write(this->m_inputOptions.outputfile);

  return;
}

Adcirc::Geometry::Mesh Interpolate::readMesh(
    const Adcirc::Output::OutputFormat &filetype) {
  Adcirc::Geometry::Mesh mesh;
  if (filetype == Adcirc::Output::OutputNetcdf3 ||
      filetype == Adcirc::Output::OutputNetcdf4) {
    if (this->m_inputOptions.readasciimesh) {
      mesh.setFilename(this->m_inputOptions.mesh);
    } else {
      if (this->m_inputOptions.mesh != std::string()) {
        Adcirc::Logging::warning(
            "Ignoring provided mesh file in favor of netCDF data",
            "[WARNING]: ");
      }
      mesh.setFilename(this->m_inputOptions.globalfile);
    }
  } else {
    mesh.setFilename(this->m_inputOptions.mesh);
  }
  mesh.read();
  return mesh;
}

Adcirc::Output::Hmdf Interpolate::readStationLocations(bool vector) {
  Adcirc::Output::Hmdf tempStn;
  if (this->m_inputOptions.readimeds) {
    std::string ext = Adcirc::FileIO::Generic::getFileExtension(
        this->m_inputOptions.stationfile);
    if (ext == ".nc") {
      tempStn.readNetcdf(this->m_inputOptions.stationfile, true);
    } else {
      tempStn.readImeds(this->m_inputOptions.stationfile);
    }
    return this->copyStationList(tempStn, vector);
  } else if (this->m_inputOptions.readdflow) {
    adcircmodules_throw_exception("D-Flow I/O not implemented");
    return Adcirc::Output::Hmdf();
  } else {
    return this->readStationList(vector);
  }
}

Adcirc::Output::Hmdf Interpolate::readStationList(bool vector) {
  Adcirc::Output::Hmdf h(vector);
  std::ifstream infile(this->m_inputOptions.stationfile);
  std::string templine;
  std::getline(infile, templine);
  size_t nStations = stoull(templine);
  for (size_t i = 0; i < nStations; ++i) {
    std::getline(infile, templine);
    std::vector<std::string> templist;
    Adcirc::FileIO::Generic::splitString(templine, templist);
    Adcirc::Output::HmdfStation s(vector);
    std::string name = boost::str(boost::format("Station_%04.4i") % (i + 1));
    s.setId(name);
    s.setName(name);
    s.setLongitude(stod(templist[0]));
    s.setLatitude(stod(templist[1]));
    s.setStationIndex(i);
    h.addStation(s);
  }
  infile.close();
  return h;
}

Adcirc::Output::Hmdf Interpolate::copyStationList(Adcirc::Output::Hmdf &list,
                                                  const bool vector) {
  Adcirc::Output::Hmdf out(vector);
  out.setHeader1(list.header1());
  out.setHeader2(list.header2());
  out.setHeader3(list.header3());
  out.setDatum(list.datum());
  out.setUnits(list.units());
  for (size_t i = 0; i < list.nstations(); ++i) {
    Adcirc::Output::HmdfStation s(vector);
    s.setId(list.station(i)->id());
    s.setName(list.station(i)->name());
    s.setStationIndex(list.station(i)->stationIndex());
    s.setLongitude(list.station(i)->coordinate()->longitude);
    s.setLatitude(list.station(i)->coordinate()->latitude);
    out.addStation(s);
  }
  return out;
}

void Interpolate::generateInterpolationWeights(Adcirc::Geometry::Mesh &m,
                                               Adcirc::Output::Hmdf &stn) {
  m.buildElementalSearchTree();
  size_t nFound = 0;
  Ezproj e;

  this->m_weights.resize(stn.nstations());

  for (size_t i = 0; i < stn.nstations(); ++i) {
    std::vector<double> wt;
    double x1 = stn.station(i)->longitude();
    double y1 = stn.station(i)->latitude();
    double x2 = 0;
    double y2 = 0;

    if (this->m_inputOptions.epsg_station != this->m_inputOptions.epsg_global) {
      bool latlon = false;
      e.transform(this->m_inputOptions.epsg_station,
                  this->m_inputOptions.epsg_global, x1, y1, x2, y2, latlon);
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
                 nFound % stn.nstations()),
      "[INFO]: ");
  if (nFound == 0) {
    Adcirc::Logging::logError(
        "No stations found within mesh for interpolation.", "[ERROR]: ");
    exit(1);
  }
  return;
}

void Interpolate::logProgress(const size_t nsnap, const size_t i) {
  double pct = (static_cast<double>(i - this->m_inputOptions.startsnap + 1) /
                static_cast<double>(nsnap)) *
               100.0;
  std::string logMessage = boost::str(
      boost::format(
          "Processing snap %6.6i of %6.6i (File Snap: %6.6i) [%6.2f%%]") %
      (i - this->m_inputOptions.startsnap + 1) % nsnap % i % pct);
  Adcirc::Logging::log(logMessage, "[INFO]: ");
}

void Interpolate::allocateStationArrays(Adcirc::Output::Hmdf &stationData) {
  for (size_t i = 0; i < stationData.nstations(); ++i) {
    stationData.station(i)->reserve(this->m_inputOptions.endsnap -
                                    this->m_inputOptions.startsnap);
  }
}

void Interpolate::reprojectStationOutput(Adcirc::Output::Hmdf &output) {
  if (this->m_inputOptions.epsg_station != this->m_inputOptions.epsg_output) {
    Ezproj e;
    for (size_t i = 0; i < output.nstations(); ++i) {
      double x1 = output.station(i)->longitude();
      double y1 = output.station(i)->latitude();
      double x2 = 0;
      double y2 = 0;
      bool latlon = false;
      e.transform(this->m_inputOptions.epsg_station,
                  this->m_inputOptions.epsg_output, x1, y1, x2, y2, latlon);
      output.station(i)->setLongitude(x2);
      output.station(i)->setLatitude(y2);
    }
  }
  return;
}

void Interpolate::validatePositiveFlowDirections(
    Adcirc::Output::Hmdf &stationData) {
  if (this->m_inputOptions.positive_direction != std::vector<double>()) {
    if (this->m_inputOptions.positive_direction.size() == 1) {
      double pd = this->m_inputOptions.positive_direction[0];
      this->m_inputOptions.positive_direction.resize(stationData.nstations());
      std::fill(this->m_inputOptions.positive_direction.begin(),
                this->m_inputOptions.positive_direction.end(), pd);
    } else if (this->m_inputOptions.positive_direction.size() !=
               stationData.nstations()) {
      Adcirc::Logging::logError(
          "Number of stations and number positive directions provided do not "
          "match.",
          "[ERROR]: ");
    }
  }
}

Date Interpolate::getColdstartDate() {
  Adcirc::Output::Hmdf::HmdfFileType ft =
      Adcirc::Output::Hmdf::getFiletype(this->m_inputOptions.outputfile);
  if (ft == Adcirc::Output::Hmdf::HmdfImeds ||
      ft == Adcirc::Output::Hmdf::HmdfCsv ||
      ft == Adcirc::Output::Hmdf::HmdfNetCdf) {
    return this->dateFromString(this->m_inputOptions.coldstart);
  }
  return Date();
}

void Interpolate::interpolateTimeSnapToStations(
    const size_t snap, const bool writeVector, const bool hasDirection,
    const Date &coldstart, Adcirc::Output::ReadOutput &globalFile,
    Adcirc::Output::Hmdf &stationData) {
  globalFile.read(snap - 1);
  Date d = coldstart;
  d.add(static_cast<long>(globalFile.dataAt(0)->time()));
  long long datetime = d.toMSeconds();

  for (size_t j = 0; j < stationData.nstations(); ++j) {
    if (this->m_weights[j].found) {
      if (writeVector) {
        stationData.station(j)->setNext(
            datetime, this->interpVector(globalFile, this->m_weights[j]));
      } else {
        if (hasDirection) {
          stationData.station(j)->setNext(
              datetime,
              this->interpScalar(globalFile, this->m_weights[j],
                                 this->m_inputOptions.positive_direction[j]));
        } else {
          stationData.station(j)->setNext(
              datetime, this->interpScalar(globalFile, this->m_weights[j]));
        }
      }
    } else {
      if (writeVector) {
        stationData.station(j)->setNext(datetime, globalFile.defaultValue(),
                                        globalFile.defaultValue());
      } else {
        stationData.station(j)->setNext(datetime, globalFile.defaultValue());
      }
    }
  }
  return;
}

double Interpolate::interpScalar(Adcirc::Output::ReadOutput &data, Weight &w,
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

std::tuple<double, double> Interpolate::interpVector(
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

double Interpolate::interpScalarFromVectorWithFlowDirection(
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

double Interpolate::interpScalarFromVectorWithoutFlowDirection(
    Adcirc::Output::ReadOutput &data, Weight &w) {
  double v1 = data.dataAt(0)->magnitude(w.node_index[0]);
  double v2 = data.dataAt(0)->magnitude(w.node_index[1]);
  double v3 = data.dataAt(0)->magnitude(w.node_index[2]);
  return this->interpolateDryValues(v1, w.weight[0], v2, w.weight[1], v3,
                                    w.weight[2], data.defaultValue());
}

double Interpolate::interpDirectionFromVector(Adcirc::Output::ReadOutput &data,
                                              Weight &w) {
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

double Interpolate::interpScalarFromVector(Adcirc::Output::ReadOutput &data,
                                           Weight &w,
                                           const double positive_direction) {
  using namespace Adcirc::FpCompare;
  if (this->m_inputOptions.magnitude && equalTo(positive_direction, -9999.0)) {
    return this->interpScalarFromVectorWithoutFlowDirection(data, w);
  } else if (this->m_inputOptions.magnitude &&
             !equalTo(positive_direction, -9999.0)) {
    return this->interpScalarFromVectorWithFlowDirection(data, w,
                                                         positive_direction);
  } else if (this->m_inputOptions.direction) {
    return this->interpDirectionFromVector(data, w);
  } else {
    adcircmodules_throw_exception(
        "Cannot write vector data. Select --magnitude or --direction");
    return data.defaultValue();
  }
}

double Interpolate::interpolateDryValues(double v1, double w1, double v2,
                                         double w2, double v3, double w3,
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

Date Interpolate::dateFromString(const std::string &dateString) {
  int year = stoi(dateString.substr(0, 4));
  int month = stoi(dateString.substr(4, 2));
  int day = stoi(dateString.substr(6, 2));
  int hour = stoi(dateString.substr(8, 2));
  int minute = stoi(dateString.substr(10, 2));
  int second = stoi(dateString.substr(12, 2));
  return Date(year, month, day, hour, minute, second);
}
