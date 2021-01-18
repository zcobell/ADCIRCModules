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

/* Adcirc Interface File */
%module pyadcircmodules

%insert("python") %{
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
%}

%{
#define SWIG_FILE_WITH_INIT
#include "AdcircModules_Global.h"
#include "Config.h"
#include "Logging.h"
#include "FileTypes.h"
#include "AdcHash.h"
#include "HashType.h"
#include "Mesh.h"
#include "CDate.h"
#include "Hmdf.h"
#include "HmdfStation.h"
#include "StationInterpolationOptions.h"
#include "StationInterpolation.h"
#include "Node.h"
#include "Element.h"
#include "Boundary.h"
#include "Topology.h"
#include "ElementTable.h"
#include "NodeTable.h"
#include "FaceTable.h"
#include "Attribute.h"
#include "AttributeMetadata.h"
#include "NodalAttributes.h"
#include "OutputMetadata.h"
#include "ReadOutput.h"
#include "WriteOutput.h"
#include "OutputRecord.h"
#include "HarmonicsRecord.h"
#include "HarmonicsOutput.h"
#include "KDTree.h"
#include "Projection.h"
#include "Meshchecker.h"
#include "Multithreading.h"
#include "Constants.h"
#include "Point.h"
#include "PixelValue.h"
#include "PixelValueVector.h"
%}

#ifdef _USE_GDAL
%{
#include "Griddata.h"
#include "InterpolationMethods.h"
%}
#endif

%include <std_string.i>
%include <exception.i>
%include <std_vector.i>
%include <windows.i>

%exception {
  try {
    $action
  } catch (const std::exception& e) {
    SWIG_exception(SWIG_RuntimeError, e.what());
  } catch (const std::string& e) {
    SWIG_exception(SWIG_RuntimeError, e.c_str());
  }
}

namespace std {
    %template(IntVector) vector<int>;
    %template(SizetVector) vector<size_t>;
    %template(DoubleVector) vector<double>;
    %template(DoubleDoubleVector) vector<vector<double>>;
    %template(SizetSizetVector) vector<vector<size_t>>;
    %template(NodeVector) vector<Adcirc::Geometry::Node*>;
    %template(DateVector) vector<Adcirc::CDate>;
}

%include "AdcircModules_Global.h"
%include "Config.h"
%include "Logging.h"
%include "FileTypes.h"
%include "AdcHash.h"
%include "HashType.h"
%include "Mesh.h"
%include "CDate.h"
%include "Hmdf.h"
%include "HmdfStation.h"
%include "StationInterpolationOptions.h"
%include "StationInterpolation.h"
%include "Node.h"
%include "Element.h"
%include "Boundary.h"
%include "Topology.h"
%include "ElementTable.h"
%include "NodeTable.h"
%include "FaceTable.h"
%include "Attribute.h"
%include "AttributeMetadata.h"
%include "NodalAttributes.h"
%include "OutputMetadata.h"
%include "ReadOutput.h"
%include "WriteOutput.h"
%include "OutputRecord.h"
%include "HarmonicsRecord.h"
%include "HarmonicsOutput.h"
%include "KDTree.h"
%include "Projection.h"
%include "Meshchecker.h"
%include "Multithreading.h"
%include "Constants.h"
%include "Point.h"
%include "PixelValue.h"
%include "PixelValueVector.h"

#ifdef _USE_GDAL
%include "Griddata.h"
%include "InterpolationMethods.h"
#endif

