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
#include "griddata.h"

#include "griddata_private.h"

using namespace Adcirc::Interpolation;

/**
 * @brief Constructor which uses discrete point locations
 * @param[in] x vector of query x locations
 * @param[in] y vector of query y locations
 * @param[in] resolution vector of representitive sizes near these points. Used
 * for various sizing functions
 * @param[in] rasterFile gdal compatible raster file name
 * @param[in] epsgQuery coordinate system for the input point data
 * @param[in] epsgRaster coordinate system for the raster data
 */
Griddata::Griddata(const std::vector<double> &x, const std::vector<double> &y,
                   std::vector<double> resolution,
                   const std::string &rasterFile, int epsgQuery, int epsgRaster)
    : m_impl(std::make_unique<Adcirc::Private::GriddataPrivate>(
          x, y, std::move(resolution), rasterFile, epsgQuery, epsgRaster)) {}

/**
 * @brief Constructor that takes mesh and raster file
 * @param[in] mesh pointer to mesh object
 * @param[in] rasterFile gdal compatible raster file name
 * @param[in] epsgRaster coordinate system for the raster data
 */
Griddata::Griddata(Adcirc::Geometry::Mesh *mesh, const std::string &rasterFile,
                   int epsgRaster)
    : m_impl(std::make_unique<Adcirc::Private::GriddataPrivate>(
          mesh, rasterFile, epsgRaster)) {}

/**
 * @brief Retrieves the filename of the raster currently being used for
 * interpolation
 * @return raster file name
 */
std::string Griddata::rasterFile() const { return this->m_impl->rasterFile(); }
void Griddata::setRasterFile(const std::string &rasterFile) {
  this->m_impl->setRasterFile(rasterFile);
}

/**
 * @brief Reads a lookup table to be used with the interpolation
 * @param[in] lookupTableFile name of lookup table
 *
 * The format for the lookup table should be:
 *
 * [key_1]   [value_1]   [description](optional)
 * [key_2]   [value_2]   [description](optional)
 *   .
 *   .
 *   .
 * [key_n]   [value_3]   [description](optional)
 *
 * The key and value fields are required. The description
 * is not used by the code but does make the tables more
 * understandable when being read by a human. The key
 * value must be an integer and the value must be a floating
 * point number.
 */
void Griddata::readLookupTable(const std::string &lookupTableFile) {
  this->m_impl->readLookupTable(lookupTableFile);
}

/**
 * @brief Returns the current interpolation flags for all nodes being used by
 * the code. By default all interpolation flags are set to
 * Interpolation::Average
 * @return Vector of interpolation flags
 */
std::vector<int> Griddata::interpolationFlags() const {
  return this->m_impl->interpolationFlags();
}

/**
 * @brief Sets the interpolation flags to a vector. The vector must be equal
 * length to the number of nodes in the mesh
 * @param[in] interpolationFlags Vector of flags equal to the number of nodes in
 * the mesh
 */
void Griddata::setInterpolationFlags(
    const std::vector<int> &interpolationFlags) {
  this->m_impl->setInterpolationFlags(interpolationFlags);
}

/**
 * @brief Sets the interpolation flags to a uniform value
 * @param[in] interpolationFlag flag that will be set for all nodes
 */
void Griddata::setInterpolationFlags(int interpolationFlag) {
  this->m_impl->setInterpolationFlags(interpolationFlag);
}

/**
 * @brief Retrieves the interpolation flag for a specified node
 * @param[in] index position to retrieve flag for
 * @return interpolation flag
 */
int Griddata::interpolationFlag(size_t index) {
  return this->m_impl->interpolationFlag(index);
}

/**
 * @brief Sets the interpolation flag at a specified node
 * @param[in] index node position to set the flag
 * @param[in] flag interpolation method to use
 */
void Griddata::setInterpolationFlag(size_t index, int flag) {
  this->m_impl->setInterpolationFlag(index, flag);
}

/**
 * @brief Returns the sizes of the relative window (mean element size around
 * node
 * * [x]) used to identify pixels of interest when interpolating data to the
 * mesh
 * @return mesh size multiplier
 */
std::vector<double> Griddata::filterSizes() const {
  return this->m_impl->filterSizes();
}

/**
 * @brief Sets the relative filter sizes (mean element size around node * [x])
 * used to identify pixels of interest when interpolating data to the mesh
 * @param filterSize mesh size multiplier
 */
void Griddata::setFilterSizes(const std::vector<double> &filterSize) {
  this->m_impl->setFilterSizes(filterSize);
}

/**
 * @brief Sets the relative filter sizes used to identify pixels of interest
 * when interpolating data to a mesh. All nodes set to a uniform value
 * @param[in] filterSize mesh size multiplier
 */
void Griddata::setFilterSizes(double filterSize) {
  this->m_impl->setFilterSizes(filterSize);
}

/**
 * @brief Returns the filter size (mean element size around node * [x]) at a
 * specified index
 * @param[in] index node position
 * @return mesh size multiplier
 */
double Griddata::filterSize(size_t index) {
  return this->m_impl->filterSize(index);
}

/**
 * @brief Sets the relative filter size (mean element size around node * [x]) at
 * a specified node
 * @param[in] index node position
 * @param[in] filterSize mesh size multiplier
 */
void Griddata::setFilterSize(size_t index, double filterSize) {
  return this->m_impl->setFilterSize(index, filterSize);
}

/**
 * @brief Returns the default value that will be applied to the output arrays
 * when no interpolation method is selected or the specified interpolation
 * method fails
 * @return default value
 */
double Griddata::defaultValue() const { return this->m_impl->defaultValue(); }

/**
 * @brief Sets the default value that will be applied to the output arrays when
 * no interpolation method is selected or the specified interpolation method
 * fails
 * @param[in] defaultValue default value to use. Default is
 * return of adcircmodules_default_value<double>()
 */
void Griddata::setDefaultValue(double defaultValue) {
  this->m_impl->setDefaultValue(defaultValue);
}

/**
 * @brief Runs the interpolation process provided a mesh and raster have been
 * supplied. Returns the interpolated values in an array matching the node
 * indidices. This process is recommended but not required to be carried out in
 * planar (i.e. UTM) coordinates
 * @param[in] useLookupTable determines if the code uses a lookup table or the
 * values from the raster directly.
 * @return Vector of interpolated values
 *
 * Note that it is possible for the code to perform better or worse depending
 * upon the system being used with the raster in memory and with parallel code
 * enabled.
 */
std::vector<double> Griddata::computeValuesFromRaster(bool useLookupTable) {
  return this->m_impl->computeValuesFromRaster(useLookupTable);
}

/**
 * @brief Computes the upwind directional wind roughness in 12 directions and
 * returns a 2d array of values indexed [1:numNodes][1:12]. This process is
 * required to be carried out in planar coordinates (i.e. UTM)
 * @param[in] useLookupTable determines if the code uses a lookup table or the
 * values from the raster directly.
 * @return 2d vector of interpolated values
 *
 * Note that the code may perform better with the raster in memory and using
 * parallel code under certain situations. This will depend on raster size
 * and disk I/O speed
 *
 */
std::vector<std::vector<double>> Griddata::computeDirectionalWindReduction(
    bool useLookupTable) {
  return this->m_impl->computeDirectionalWindReduction(useLookupTable);
}

/**
 * @brief Returns the epsg code used for the current raster. Note that this MUST
 * match the mesh's epsg code.
 * @return
 */
int Griddata::epsg() const { return this->m_impl->epsg(); }

/**
 * @brief Sets the EPSG code used for the mesh
 * @param[in] epsg coordinate system code
 */
void Griddata::setEpsg(int epsg) { this->m_impl->setEpsg(epsg); }

/**
 * @brief Status of the progress bar boolean
 * @return true if the progress bar will be shown, false if it will not
 */
bool Griddata::showProgressBar() const {
  return this->m_impl->showProgressBar();
}

/**
 * @brief Allows the user to enable or disable the progress bar
 * @param[in] showProgressBar set to true if progress bar should be shown, false
 * if not
 */
void Griddata::setShowProgressBar(bool showProgressBar) {
  this->m_impl->setShowProgressBar(showProgressBar);
}

/**
 * @brief Returns the raster multiplier applied after interpolation is complete
 * @return raster multiplier
 */
double Griddata::rasterMultiplier() const {
  return this->m_impl->rasterMultiplier();
}

/**
 * @brief Sets the raster multiplier that is applied when interpolation is
 * complete. Useful to define a unit conversion
 * @param[in] rasterMultiplier multiplier
 */
void Griddata::setRasterMultiplier(double rasterMultiplier) {
  this->m_impl->setRasterMultiplier(rasterMultiplier);
}

/**
 * @brief Returns true if the code will read the raster into memory instead of
 * reading as required from the disk
 * @return true if raster will be placed in memory
 *
 * Note that large rasters can exhaust the system memory and this should be used
 * only if the raster can safely be placed in RAM
 */
bool Griddata::rasterInMemory() const { return this->m_impl->rasterInMemory(); }

/**
 * @brief Sets to true, the code will read the raster into memory instead of
 * reading as required from disk
 * @param[in] rasterInMemory true if raster should be read into RAM
 *
 * Note that large rasters can exhaust the system memory and this should be used
 * only if the raster can safely be placed in RAM
 */
void Griddata::setRasterInMemory(bool rasterInMemory) {
  this->m_impl->setRasterInMemory(rasterInMemory);
}

/**
 * @brief Returns the datum shift that is added to the interpolated value
 * @return datum shift value
 */
double Griddata::datumShift() const { return this->m_impl->datumShift(); }

/**
 * @brief Sets a datum shift that is added to the interpolated value
 * @param[in] datumShift datum shift value
 */
void Griddata::setDatumShift(double datumShift) {
  this->m_impl->setDatumShift(datumShift);
}

/**
 * @brief Sets the thresholding method (none by default)
 * @param[in] method Thresholding method to use
 */
void Griddata::setThresholdMethod(const Interpolation::Threshold &method) {
  this->m_impl->setThresholdMethod(method);
}

/**
 * @brief Returns the thresholding method
 * @return method
 */
Adcirc::Interpolation::Threshold Griddata::thresholdMethod() const {
  return this->m_impl->thresholdMethod();
}

/**
 * @brief Sets the value to use in conjunction with thresholdMethod.
 * @param[in] value value to use for filtering
 */
void Griddata::setThresholdValue(const double &value) {
  this->m_impl->setThresholdValue(value);
}

/**
 * @brief Gets the extents of the current raster as xmin,ymin,xmax,ymax
 * @return vector of raster extents
 */
std::vector<double> Griddata::extents() const {
  return this->m_impl->extents();
}

/**
 * @brief Returns the value that is being used for thresholding
 * @return thresholding value
 */
double Griddata::thresholdValue() const {
  return this->m_impl->thresholdValue();
}
