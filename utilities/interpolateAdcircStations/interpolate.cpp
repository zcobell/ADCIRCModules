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
#include "fpcompare.h"
#include "logging.h"

Interpolate::Interpolate(const InputOptions &options)
    : m_inputOptions(options) {}

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
            "Ignoring provided mesh file in favor of netCDF data", "[WARNING]: ");
      }
      mesh.setFilename(this->m_inputOptions.globalfile);
    }
  } else {
    mesh.setFilename(this->m_inputOptions.mesh);
  }
  mesh.read();
  return mesh;
}

Adcirc::Output::Hmdf Interpolate::readStationLocations() {
  Adcirc::Output::Hmdf stn;
  if (this->m_inputOptions.readimeds) {
    std::string ext = Adcirc::FileIO::Generic::getFileExtension(
        this->m_inputOptions.stationfile);
    if (ext == "nc") {
      stn.readNetcdf(this->m_inputOptions.stationfile);
    } else {
      stn.readImeds(this->m_inputOptions.stationfile);
    }
  } else if (this->m_inputOptions.readdflow) {
    adcircmodules_throw_exception("D-Flow I/O not implemented");
  } else {
    stn = this->readStationList();
  }
  return stn;
}

Adcirc::Output::Hmdf Interpolate::readStationList() {
  Adcirc::Output::Hmdf h;
  std::ifstream infile(this->m_inputOptions.stationfile);
  std::string templine;
  std::getline(infile, templine);
  size_t nStations = stoull(templine);
  for (size_t i = 0; i < nStations; ++i) {
    std::getline(infile, templine);
    std::vector<std::string> templist;
    Adcirc::FileIO::Generic::splitString(templine, templist);
    Adcirc::Output::HmdfStation s;
    std::string name = boost::str(boost::format("Station_%04.4i") % (i + 1));
    s.setId(name);
    s.setName(name);
    s.setLongitude(stod(templist[0]));
    s.setLatitude(stod(templist[1]));
    s.setStationIndex(i);
    h.addStation(s);
  }
  return h;
}

void Interpolate::generateInterpolationWeights(Adcirc::Geometry::Mesh &m,
                                               Adcirc::Output::Hmdf &stn) {
  m.buildElementalSearchTree();
  size_t nFound = 0;

  this->m_weights.resize(stn.nstations());

  for (size_t i = 0; i < stn.nstations(); ++i) {
    std::vector<double> wt;
    size_t eidx = m.findElement(stn.station(i)->longitude(),
                                stn.station(i)->latitude(), wt);

    if (eidx == adcircmodules_default_value<size_t>()) {
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
                 nFound % stn.nstations()),"[INFO]: ");
  if (nFound == 0) {
    Adcirc::Logging::logError("No stations found for this interpolation.","[ERROR]: ");
    exit(1);
  }
  return;
}

void Interpolate::run() {
  Adcirc::Output::Hmdf stationData = this->readStationLocations();
  Adcirc::Output::ReadOutput globalFile(this->m_inputOptions.globalfile);
  globalFile.open();

  Adcirc::Output::OutputFormat filetype = globalFile.filetype();
  Adcirc::Geometry::Mesh m = this->readMesh(filetype);

  Date coldstart;
  if (this->m_inputOptions.writeimeds) {
    coldstart = this->dateFromString(this->m_inputOptions.coldstart);
  }

  Date reftime;
  if (this->m_inputOptions.writedflow) {
    reftime = this->dateFromString(this->m_inputOptions.refdate);
  }

  this->generateInterpolationWeights(m, stationData);

  for (size_t i = 0; i < globalFile.numSnaps(); ++i) {
    globalFile.read(i);
    Date d = coldstart;
    d.add(globalFile.dataAt(0)->time());
    long long datetime = d.toMSeconds();
    for (size_t j = 0; j < stationData.nstations(); ++j) {
      double value = 0;
      if (this->m_weights[j].found) {
        value = this->interp(globalFile, this->m_weights[j]);
      } else {
        value = globalFile.defaultValue();
      }
      stationData.station(j)->setNext(datetime, value);
    }
    double pct = (static_cast<double>(i + 1) /
                  static_cast<double>(globalFile.numSnaps())) *
                 100.0;
    std::string logMessage =
        boost::str(boost::format("Processing snap %6.6i of %6.6i (%6.2f%%)") %
                   (i + 1) % globalFile.numSnaps() % pct);
    Adcirc::Logging::log(logMessage,"[INFO]: ");
    globalFile.clearAt(0);
  }

  stationData.write(this->m_inputOptions.outputfile);

  return;
}

double Interpolate::interp(Adcirc::Output::ReadOutput &data, Weight &w) {
  size_t n1 = w.node_index[0];
  size_t n2 = w.node_index[1];
  size_t n3 = w.node_index[2];
  double w1 = w.weight[0];
  double w2 = w.weight[1];
  double w3 = w.weight[2];

  if (data.metadata()->isVector()) {
    if (this->m_inputOptions.magnitude) {
      double v1 = data.dataAt(0)->magnitude(n1);
      double v2 = data.dataAt(0)->magnitude(n2);
      double v3 = data.dataAt(0)->magnitude(n3);
      return this->interpolateDryValues(v1, w1, v2, w2, v3, w3,
                                        data.defaultValue());
    } else if (this->m_inputOptions.direction) {
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
      return std::atan2(vy, vx) * Adcirc::Constants::rad2deg();
    } else {
      adcircmodules_throw_exception(
          "Cannot write vector data. Select --magnitude or --direction");
      return data.defaultValue();
    }
  } else {
    double v1 = data.dataAt(0)->z(n1);
    double v2 = data.dataAt(0)->z(n2);
    double v3 = data.dataAt(0)->z(n3);
    return this->interpolateDryValues(v1, w1, v2, w2, v3, w3,
                                      data.defaultValue());
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
