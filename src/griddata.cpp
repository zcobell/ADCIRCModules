#include "griddata.h"
#include "griddataimpl.h"

using namespace Interpolation;

Griddata::Griddata() : m_impl(new Interpolation::GriddataImpl) {}

Griddata::Griddata(Adcirc::Geometry::Mesh *mesh, std::string rasterFile)
    : m_impl(new Interpolation::GriddataImpl(mesh, rasterFile)) {}

Griddata::~Griddata() { delete this->m_impl; }

std::string Griddata::rasterFile() const { return this->m_impl->rasterFile(); }
void Griddata::setRasterFile(const std::string &rasterFile) {
  this->m_impl->setRasterFile(rasterFile);
}

void Griddata::readLookupTable(const std::string &lookupTableFile) {
  this->m_impl->readLookupTable(lookupTableFile);
}

std::vector<int> Griddata::interpolationFlags() const {
  return this->m_impl->interpolationFlags();
}

void Griddata::setInterpolationFlags(
    const std::vector<int> &interpolationFlags) {
  this->m_impl->setInterpolationFlags(interpolationFlags);
}

int Griddata::interpolationFlag(size_t index) {
  return this->m_impl->interpolationFlag(index);
}

void Griddata::setInterpolationFlag(size_t index, int flag) {
  this->m_impl->setInterpolationFlag(index, flag);
}

std::vector<double> Griddata::filterSizes() const {
  return this->m_impl->filterSizes();
}

void Griddata::setFilterSizes(const std::vector<double> &filterSize) {
  this->m_impl->setFilterSizes(filterSize);
}

double Griddata::filterSize(size_t index) {
  return this->m_impl->filterSize(index);
}

void Griddata::setFilterSize(size_t index, double filterSize) {
  return this->m_impl->setFilterSize(index, filterSize);
}

double Griddata::defaultValue() const { return this->m_impl->defaultValue(); }
void Griddata::setDefaultValue(double defaultValue) {
  this->m_impl->setDefaultValue(defaultValue);
}

std::vector<double> Griddata::computeValuesFromRaster(bool useLookupTable) {
  return this->m_impl->computeValuesFromRaster(useLookupTable);
}

std::vector<std::vector<double>> Griddata::computeDirectionalWindReduction(
    bool useLookupTable) {
  return this->m_impl->computeDirectionalWindReduction(useLookupTable);
}

int Griddata::epsg() const { return this->m_impl->epsg(); }

void Griddata::setEpsg(int epsg) { this->m_impl->setEpsg(epsg); }

bool Griddata::showProgressBar() const {
  return this->m_impl->showProgressBar();
}

void Griddata::setShowProgressBar(bool showProgressBar) {
  this->m_impl->setShowProgressBar(showProgressBar);
}

double Griddata::rasterMultiplier() const {
  return this->m_impl->rasterMultiplier();
}
void Griddata::setRasterMultiplier(double rasterMultiplier) {
  this->m_impl->setRasterMultiplier(rasterMultiplier);
}

bool Griddata::rasterInMemory() const { return this->m_impl->rasterInMemory(); }
void Griddata::setRasterInMemory(bool rasterInMemory) {
  this->m_impl->setRasterInMemory(rasterInMemory);
}

double Griddata::datumShift() const { return this->m_impl->datumShift(); }

void Griddata::setDatumShift(double datumShift) {
  this->m_impl->setDatumShift(datumShift);
}
