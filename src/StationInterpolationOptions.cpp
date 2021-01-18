#include "StationInterpolationOptions.h"

#include <fstream>

#include "FileIO.h"
#include "Logging.h"
#include "StringConversion.h"
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"

using namespace Adcirc::Output;

StationInterpolationOptions::StationInterpolationOptions()
    : m_mesh(std::string()),
      m_globalfile(std::string()),
      m_stationfile(std::string()),
      m_outputfile(std::string()),
      m_coldstart(std::string()),
      m_refdate(std::string()),
      m_magnitude(false),
      m_direction(false),
      m_readasciimesh(false),
      m_startsnap(std::numeric_limits<size_t>::max()),
      m_endsnap(std::numeric_limits<size_t>::max()),
      m_epsgGlobal(4326),
      m_epsgStation(4326),
      m_epsgOutput(4326),
      m_multiplier(1.0),
      m_hasPositiveDirection(false) {}

std::string StationInterpolationOptions::mesh() const { return this->m_mesh; }

void StationInterpolationOptions::setMesh(const std::string &mesh) {
  this->m_mesh = mesh;
}

std::string StationInterpolationOptions::globalfile() const {
  return this->m_globalfile;
}

void StationInterpolationOptions::setGlobalfile(const std::string &globalfile) {
  this->m_globalfile = globalfile;
}

std::string StationInterpolationOptions::stationfile() const {
  return this->m_stationfile;
}

void StationInterpolationOptions::setStationfile(
    const std::string &stationfile) {
  this->m_stationfile = stationfile;
}

std::string StationInterpolationOptions::outputfile() const {
  return this->m_outputfile;
}

void StationInterpolationOptions::setOutputfile(const std::string &outputfile) {
  this->m_outputfile = outputfile;
}

std::string StationInterpolationOptions::coldstart() const {
  return this->m_coldstart;
}

void StationInterpolationOptions::setColdstart(const std::string &coldstart) {
  this->m_coldstart = coldstart;
}

std::string StationInterpolationOptions::refdate() const {
  return this->m_refdate;
}

void StationInterpolationOptions::setRefdate(const std::string &refdate) {
  this->m_refdate = refdate;
}

bool StationInterpolationOptions::magnitude() const {
  return this->m_magnitude;
}

void StationInterpolationOptions::setMagnitude(bool magnitude) {
  this->m_magnitude = magnitude;
}

bool StationInterpolationOptions::direction() const {
  return this->m_direction;
}

void StationInterpolationOptions::setDirection(bool direction) {
  this->m_direction = direction;
}

bool StationInterpolationOptions::readasciimesh() const {
  return this->m_readasciimesh;
}

void StationInterpolationOptions::setReadasciimesh(bool readasciimesh) {
  this->m_readasciimesh = readasciimesh;
}

size_t StationInterpolationOptions::startsnap() const {
  return this->m_startsnap;
}

void StationInterpolationOptions::setStartsnap(const size_t &startsnap) {
  this->m_startsnap = startsnap;
}

size_t StationInterpolationOptions::endsnap() const { return this->m_endsnap; }

void StationInterpolationOptions::setEndsnap(const size_t &endsnap) {
  this->m_endsnap = endsnap;
}

int StationInterpolationOptions::epsgGlobal() const {
  return this->m_epsgGlobal;
}

void StationInterpolationOptions::setEpsgGlobal(int epsgGlobal) {
  this->m_epsgGlobal = epsgGlobal;
}

int StationInterpolationOptions::epsgStation() const {
  return this->m_epsgStation;
}

void StationInterpolationOptions::setEpsgStation(int epsgStation) {
  this->m_epsgStation = epsgStation;
}

int StationInterpolationOptions::epsgOutput() const {
  return this->m_epsgOutput;
}

void StationInterpolationOptions::setEpsgOutput(int epsgOutput) {
  this->m_epsgOutput = epsgOutput;
}

double StationInterpolationOptions::multiplier() const {
  return this->m_multiplier;
}

void StationInterpolationOptions::setMultiplier(double multiplier) {
  this->m_multiplier = multiplier;
}

void StationInterpolationOptions::readStations(const std::string &stationFile) {
  std::string stn;
  if (stationFile != std::string()) {
    stn = stationFile;
  } else {
    stn = this->m_stationfile;
  }

  std::string ext = Adcirc::FileIO::Generic::getFileExtension(stn);
  if (!Adcirc::FileIO::Generic::fileExists(stn)) {
    adcircmodules_throw_exception("Station file does not exist.");
  }

  boost::algorithm::to_lower(ext);

  if (ext == ".nc") {
    this->m_hmdf.readNetcdf(stn, true);
  } else if (ext == ".imeds") {
    Adcirc::Output::Hmdf tmpSta;
    tmpSta.readImeds(stn);
    this->m_hmdf.copyStationList(tmpSta);
  } else {
    this->m_hmdf = this->readStationList(stn);
  }
}

Adcirc::Output::HmdfStation *StationInterpolationOptions::station(
    size_t index) {
  assert(index < this->m_hmdf.nstations());
  if (index < this->m_hmdf.nstations()) {
    return this->m_hmdf.station(index);
  } else {
    adcircmodules_throw_exception("Station index out of bounds");
    return nullptr;
  }
}

Hmdf *StationInterpolationOptions::stations() { return &this->m_hmdf; }

void StationInterpolationOptions::usePositiveDirection(
    bool usePositiveDirection) {
  this->m_hasPositiveDirection = usePositiveDirection;
}

Adcirc::Output::Hmdf StationInterpolationOptions::readStationList(
    const std::string &station) {
  Adcirc::Output::Hmdf h;
  std::ifstream infile(station);
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
    s.setLongitude(std::stod(templist[0]));
    s.setLatitude(std::stod(templist[1]));
    s.setStationIndex(i);
    h.addStation(s);
  }
  infile.close();
  return h;
}

bool StationInterpolationOptions::hasPositiveDirection() const {
  return this->m_hasPositiveDirection;
}

void StationInterpolationOptions::readPositiveDirection(
    const std::string &positiveDirections) {
  std::ifstream file(positiveDirections);
  std::vector<double> v;
  for (size_t i = 0; i < this->m_hmdf.nstations(); ++i) {
    std::string line;
    bool ok;
    std::getline(file, line);
    if (file.eof()) break;
    double pd = Adcirc::StringConversion::stringToDouble(line, ok);
    if (!ok) {
      Adcirc::Logging::logError("Could not read the positive directions file",
                                "[ERROR]: ");
    } else {
      this->m_hmdf.station(i)->setPositiveDirection(pd);
    }
  }
  this->m_hasPositiveDirection = true;
  return;
}

void StationInterpolationOptions::setPositiveDirection(
    size_t index, double positiveDirection) {
  assert(index < this->m_hmdf.nstations());
  this->m_hmdf.station(index)->setPositiveDirection(positiveDirection);
  this->m_hasPositiveDirection = true;
}

void StationInterpolationOptions::setPositiveDirections(
    double positiveDirection) {
  for (size_t i = 0; i < this->m_hmdf.nstations(); ++i) {
    this->m_hmdf.station(i)->setPositiveDirection(positiveDirection);
  }
  this->m_hasPositiveDirection = true;
}
