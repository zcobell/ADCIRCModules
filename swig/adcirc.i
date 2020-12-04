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
#include "adcircmodules_global.h"
#include "config.h"
#include "logging.h"
#include "filetypes.h"
#include "adchash.h"
#include "hashtype.h"
#include "mesh.h"
#include "cdate.h"
#include "hmdf.h"
#include "hmdfstation.h"
#include "stationinterpolationoptions.h"
#include "stationinterpolation.h"
#include "node.h"
#include "element.h"
#include "boundary.h"
#include "topology.h"
#include "elementtable.h"
#include "nodetable.h"
#include "facetable.h"
#include "attribute.h"
#include "attributemetadata.h"
#include "nodalattributes.h"
#include "outputmetadata.h"
#include "readoutput.h"
#include "writeoutput.h"
#include "outputrecord.h"
#include "harmonicsrecord.h"
#include "harmonicsoutput.h"
#include "kdtree.h"
#include "projection.h"
#include "meshchecker.h"
#include "multithreading.h"
#include "constants.h"
%}

#ifdef _USE_GDAL
%{
#include "griddata.h"
#include "interpolationmethods.h"
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

%include "adcircmodules_global.h"
%include "config.h"
%include "logging.h"
%include "filetypes.h"
%include "adchash.h"
%include "hashtype.h"
%include "mesh.h"
%include "cdate.h"
%include "hmdf.h"
%include "hmdfstation.h"
%include "stationinterpolationoptions.h"
%include "stationinterpolation.h"
%include "node.h"
%include "element.h"
%include "boundary.h"
%include "topology.h"
%include "elementtable.h"
%include "facetable.h"
%include "nodetable.h"
%include "attribute.h"
%include "attributemetadata.h"
%include "nodalattributes.h"
%include "readoutput.h"
%include "writeoutput.h"
%include "outputrecord.h"
%include "outputmetadata.h"
%include "harmonicsrecord.h"
%include "harmonicsoutput.h"
%include "kdtree.h"
%include "projection.h"
%include "meshchecker.h"
%include "multithreading.h"
%include "constants.h"
#ifdef _USE_GDAL
%include "griddata.h"
%include "interpolationmethods.h"
#endif

