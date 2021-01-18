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
#include "Griddata.h"

#include "GriddataPrivate.h"

using namespace Adcirc::Interpolation;

/**
 * @brief Constructor which uses discrete point locations
 * @param[in] x vector of query x locations
 * @param[in] y vector of query y locations
 * @param[in] resolution vector of representative sizes near these points. Used
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
std::vector<Adcirc::Interpolation::Method> Griddata::interpolationFlags()
    const {
  return this->m_impl->interpolationFlags();
}

/**
 * @brief Sets the interpolation flags to a vector. The vector must be equal
 * length to the number of nodes in the mesh
 * @param[in] interpolationFlags Vector of flags equal to the number of nodes in
 * the mesh
 */
void Griddata::setInterpolationFlags(
    const std::vector<Adcirc::Interpolation::Method> &interpolationFlags) {
  this->m_impl->setInterpolationFlags(interpolationFlags);
}

/**
 * @brief Sets the interpolation flags to a uniform value
 * @param[in] interpolationFlag flag that will be set for all nodes
 */
void Griddata::setInterpolationFlags(
    Adcirc::Interpolation::Method interpolationFlag) {
  this->m_impl->setInterpolationFlags(interpolationFlag);
}

/**
 * @brief Retrieves the interpolation flag for a specified node
 * @param[in] index position to retrieve flag for
 * @return interpolation flag
 */
Adcirc::Interpolation::Method Griddata::interpolationFlag(size_t index) {
  return this->m_impl->interpolationFlag(index);
}

/**
 * @brief Sets the interpolation flag at a specified node
 * @param[in] index node position to set the flag
 * @param[in] flag interpolation method to use
 */
void Griddata::setInterpolationFlag(size_t index,
                                    Adcirc::Interpolation::Method flag) {
  this->m_impl->setInterpolationFlag(index, flag);
}

/**
 * @brief Returns the full set of backup interpolation flags
 * @return vector of backup interpolation flags
 */
std::vector<Adcirc::Interpolation::Method> Griddata::backupInterpolationFlags()
    const {
  return this->m_impl->backupInterpolationFlags();
}

/**
 * @brief Griddata::setBackupInterpolationFlags
 * @param interpolationFlags
 */
void Griddata::setBackupInterpolationFlags(
    const std::vector<Adcirc::Interpolation::Method> &interpolationFlags) {
  this->m_impl->setBackupInterpolationFlags(interpolationFlags);
}

void Griddata::setBackupInterpolationFlags(
    Adcirc::Interpolation::Method interpolationFlags) {
  this->m_impl->setBackupInterpolationFlags(interpolationFlags);
}

Adcirc::Interpolation::Method Griddata::backupInterpolationFlag(size_t index) {
  return this->m_impl->backupInterpolationFlag(index);
}

void Griddata::setBackupInterpolationFlag(size_t index,
                                          Adcirc::Interpolation::Method flag) {
  this->m_impl->setBackupInterpolationFlag(index, flag);
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
 * indices. This process is recommended but not required to be carried out in
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
* Example usage provided by Keith Roberts
* 
* @section Prerequisites
*   -# Land use data, example: <a href='https://coast.noaa.gov/digitalcoast/data/ccapregional.html'>CCAP</a>
*   -# mesh file
*   -# fort13 file which contains the nodal attribute
*        @code
*        surface_directional_effective_roughness_length 
*        m
*        12
*        0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0 0.0
*        surface_directional_effective_roughness_length
*        0
*        @endcode
*   -# Lookup Table (optional) - This is a table mapping land use class integers to values. If your raster already has values embedded, you can skip this
*        @code
*        2    0.500     :High Intensity Developed
*        3    0.390     :Medium Intensity Developed
*        4    0.500     :Low Intensity Developed
*        5    0.330     :Developed Open Space
*        6    0.060     :Cultivated Land
*        7    0.060     :Pasture/Hay
*        8    0.040     :Grassland
*        9    0.650     :Deciduous Forest
*       10    0.720     :Evergreen Forest
*       11    0.710     :Mixed Forest
*       12    0.120     :Scrub/Shrub
*       13    0.550     :Palustrine Forested Wetland
*       14    0.110     :Palustrine Scrub/Shrub Wetland
*       15    0.110     :Palustrine Emergent Wetland
*       16    0.550     :Estuarine Forested Wetland
*       17    0.120     :Estuarine Scrub/Shrub Wetland
*       18    0.110     :Estuarine Emergent Wetland
*       19    0.090     :Unconsolidated Shore
*       20    0.090     :Bare Land
*       21    0.001     :Open Water
*       22    0.040     :Palustrine Aquatic Bed
*       23    0.040     :Estuarine Aquatic Bed
*       @endcode
* @endsection
*
* @section build Building and Setup 
* 
* The code can be compiled to use the ADCIRCModules library using the following:
* @code
* g++ -I /usr/local/include/adcircmodules/  -ladcircmodules -std=c++14 directional_wind_reduction.cpp -o directional_wind_reduction
* @endcode
* Note that the actual location of your include files may vary depending on your installation
*
* You may want to test the code using only a few nodes at first. This can be done by turning of the interpolation flags as shown
* in the below c++ example. Because this calculation is time intensive, it is better to discover errors in your setup earlier than
* later. Note that this calculation can be performed using parallel processing (OpenMP) if it was enabled at compile time.
*
* The land use raster data should be in a meters based coordinate system. While many other processes in ADCIRCModules are agnostic,
* this process requires the use of a meters based coordinate system. I typically use UTM. You can convert data to the coordinate 
* system using GDAL.
*
* @code
* gdalwarp -t_srs ESPG:21918 source.tif output.tif
* @endcode
* 
* With these steps complete, you can run your executable. A progress bar will show the progress and (if using a terminal) estimated
* time remaining.
* 
* @endsection
*
* @section Output
*
* When complete, you can use the <a href="https://github.com/CHLNDDEV/OceanMesh2D">OceanMesh</a> project scripts to visualize your data.
* 
* @code
* m = msh('fname','fort.14','aux',{'myfort.13'}); 
* plot(m,'type','dir','proj','lamb'); 
* @endcode
* 
* The output will look something like:
* Note that by default it plots the infinity norm at each point (the largest directional wind stress reduction at each node of the grid).
*
* @image html sderl_01.png width=800px
* @image html sderl_02.png width=800px
*
* @endsection
* 
* @example directional_wind_reduction.cpp
*
* There is an equivalent python example provided here
* @include directional_wind_reduction.py
*
*/ 

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
 * @brief Returns the wind radius used. 10km
 */
double Griddata::windRadius() const { return this->m_impl->windRadius(); }

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
