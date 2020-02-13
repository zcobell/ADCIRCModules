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
#include "adcircmodules.h"
#include "boost/format.hpp"
#include "cxxopts.hpp"
#include "fpcompare.h"
#include "highwatermarkoptions.h"
#include "hwmstats.h"
#include "locations.h"
#include "shapefil.h"

HighWaterMarkOptions parseCommandLineOptions(
    const cxxopts::ParseResult &parser);

Locations generateHighWaterMarks(const HighWaterMarkOptions &options);
std::pair<double, double> findNearestWet(const double x, const double y,
                                         Adcirc::Geometry::Mesh &mesh,
                                         Adcirc::Output::OutputRecord *record,
                                         const double maxDist,
                                         const size_t maxSearchDepth);
void writeOutput(Locations &hwm, const HighWaterMarkOptions &options);
void writeShapefile(Locations &hwm, const HighWaterMarkOptions &options);
void writeCsv(Locations &hwm, const HighWaterMarkOptions &options);

int main(int argc, char *argv[]) {
  cxxopts::Options options("highWaterMarks",
                           "ADCIRC High Water Mark Extraction");

  // clang-format off
  options.add_options() ("h,help", "show this message")
                     ("v,version","prints the adcircmodules version this code is based on")
                     ("mesh", "specify the ADCIRC geometry used in the global file",cxxopts::value<std::string>())
                     ("maxele","specify the ADCIRC maximum file",cxxopts::value<std::string>())
                     ("station", "specify the locations of the high water marks (csv, shp)",cxxopts::value<std::string>())
                     ("output", "specify the output file to be created",cxxopts::value<std::string>())
                     ("distance","maximum distance a high water mark can be moved (meters)",cxxopts::value<double>()->default_value("0"))
                     ("stats","show high water mark statistics when complete")
                     ("zero","force high water mark trendline through zero")
                     ("field","shapefile fieldname for elevation",cxxopts::value<std::string>()->default_value("observed"))
                     ("inunits","units contained in the hwm dataset. ft or m",cxxopts::value<std::string>()->default_value("m"))
                     ("outunits","units contained in the output data. ft or m",cxxopts::value<std::string>()->default_value("m"))
                     ("sdepth","Number of nearest nodes to search for when finding nearest non-dry location. Default: Number of mesh nodes (Warning: slow)",cxxopts::value<size_t>());
  // clang-format on

  if (argc == 1) {
    Adcirc::Logging::logError("No options specified");
    std::cout << options.help() << std::endl;
    return 1;
  }

  auto result = options.parse(argc, argv);

  if (result["help"].count() > 0) {
    std::cout << options.help();
    return 1;
  }

  HighWaterMarkOptions option = parseCommandLineOptions(result);

  Locations hwm = generateHighWaterMarks(option);

  writeOutput(hwm, option);

  if (option.stats()) {
    HwmStats stat(hwm, option.zero());
    stat.print();
  }

  return 0;
}

bool exists(const std::string &filename) {
  std::ifstream ff(filename);
  return ff.good();
}

HighWaterMarkOptions parseCommandLineOptions(
    const cxxopts::ParseResult &parser) {
  HighWaterMarkOptions myOptions(parser);

  if (myOptions.maxele() == std::string()) {
    Adcirc::Logging::logError("Must supply maxele file");
    exit(1);
  }
  if (!exists(myOptions.maxele())) {
    Adcirc::Logging::logError("Maxele file does not exist.");
    exit(1);
  }

  Adcirc::Output::ReadOutput gbl(myOptions.maxele());
  gbl.open();
  gbl.close();

  if (gbl.filetype() == Adcirc::Output::OutputAsciiFull ||
      gbl.filetype() == Adcirc::Output::OutputAsciiSparse) {
    if (myOptions.mesh() == std::string()) {
      Adcirc::Logging::logError(
          "Ascii file format requires mesh geometry input");
      exit(1);
    } else if (!exists(myOptions.mesh())) {
      Adcirc::Logging::logError("Mesh file does not exist.");
      exit(1);
    }
  } else {
    myOptions.setMesh(myOptions.maxele());
  }

  if (myOptions.station() == std::string()) {
    Adcirc::Logging::logError("Must supply high water mark locations");
    exit(1);
  } else if (!exists(myOptions.station())) {
    Adcirc::Logging::logError("Location file does not exist");
    exit(1);
  }

  if (myOptions.output() == std::string()) {
    Adcirc::Logging::logError("Must supply output filename");
  }

  return myOptions;
}

Locations generateHighWaterMarks(const HighWaterMarkOptions &options) {
  Adcirc::Geometry::Mesh mesh(options.mesh());
  Adcirc::Output::ReadOutput maxele(options.maxele());

  mesh.read();
  maxele.open();
  maxele.read();
  maxele.close();
  mesh.buildNodalSearchTree();

  if (mesh.numNodes() != maxele.numNodes()) {
    Adcirc::Logging::logError("Mesh and maxele have differing number of nodes");
    exit(1);
  }

  Locations loc(options.station(), options.field(), options.units());
  loc.read();

  size_t numNotFound = 0;
  for (size_t i = 0; i < loc.size(); ++i) {
    std::vector<double> weight;
    size_t idx =
        mesh.findElement(loc.location(i)->x(), loc.location(i)->y(), weight);

    if (idx == Adcirc::Geometry::Mesh::ELEMENT_NOT_FOUND) {
      numNotFound++;
      loc.location(i)->weighting()->found = false;
    } else {
      loc.location(i)->weighting()->weight = {weight[0], weight[1], weight[2]};
      loc.location(i)->weighting()->node_index = {
          mesh.element(idx)->node(0)->id() - 1,
          mesh.element(idx)->node(1)->id() - 1,
          mesh.element(idx)->node(2)->id() - 1};
      loc.location(i)->weighting()->found = true;
    }
  }

  Adcirc::Logging::log(
      boost::str(boost::format("Found %d high water marks inside mesh") %
                 (loc.size() - numNotFound)));
  Adcirc::Logging::log(boost::str(
      boost::format("Found %d high water marks outside mesh") % numNotFound));

  size_t nRewetted = 0;
  for (size_t i = 0; i < loc.size(); ++i) {
    double v1;
    double moveDist = 0.0;
    if (loc.location(i)->weighting()->found) {
      const double z1 =
          maxele.dataAt(0)->z(loc.location(i)->weighting()->node_index[0]);
      const double z2 =
          maxele.dataAt(0)->z(loc.location(i)->weighting()->node_index[1]);
      const double z3 =
          maxele.dataAt(0)->z(loc.location(i)->weighting()->node_index[2]);
      const double w1 = loc.location(i)->weighting()->weight[0];
      const double w2 = loc.location(i)->weighting()->weight[1];
      const double w3 = loc.location(i)->weighting()->weight[2];

      v1 = Adcirc::Output::StationInterpolation::interpolateDryValues(
          z1, z2, z3, w1, w2, w3, Adcirc::Output::defaultOutputValue());
    } else {
      v1 = Adcirc::Output::defaultOutputValue();
    }

    if (Adcirc::FpCompare::equalTo(v1, Adcirc::Output::defaultOutputValue()) &&
        options.distance() > 0.0) {
      std::tie(v1, moveDist) = findNearestWet(
          loc.location(i)->x(), loc.location(i)->y(), mesh, maxele.dataAt(0),
          options.distance(), options.searchDepth());
      if (!Adcirc::FpCompare::equalTo(moveDist, 0.0)) {
        nRewetted++;
      }
    }
    loc.location(i)->setModeled(v1);
    loc.location(i)->setMovedDist(moveDist);
  }

  Adcirc::Logging::log(boost::str(
      boost::format(
          "Rewetted %d locations using nearest wet location within %0.2f meters") %
      nRewetted % options.distance()));

  return loc;
}

std::pair<double, double> findNearestWet(const double x, const double y,
                                         Adcirc::Geometry::Mesh &mesh,
                                         Adcirc::Output::OutputRecord *record,
                                         const double maxDist,
                                         const size_t maxSearchDepth) {
  Adcirc::Kdtree *tree = mesh.nodalSearchTree();
  std::vector<size_t> listing =
      tree->findXNearest(x, y, std::min(maxSearchDepth, mesh.numNodes()));
  double value, distance;
  for (auto &n : listing) {
    double x_n = mesh.node(n)->x();
    double y_n = mesh.node(n)->y();
    distance = Adcirc::Constants::distance(x_n, y_n, x, y, true);
    value = record->z(n);
    if (distance < maxDist && value > record->defaultValue()) {
      return std::pair<double, double>(value, distance);
    } else if (distance > maxDist) {
      return std::pair<double, double>(record->defaultValue(), 0);
    }
  }
  return std::pair<double, double>(record->defaultValue(), 0);
}

void writeOutput(Locations &hwm, const HighWaterMarkOptions &options) {
  std::string extension =
      Adcirc::FileIO::Generic::getFileExtension(options.output());
  if (extension == ".shp") {
    writeShapefile(hwm, options);
  } else if (extension == ".csv") {
    writeCsv(hwm, options);
  } else {
    Adcirc::Logging::logError("Unknown output format");
    exit(1);
  }
  return;
}

void writeShapefile(Locations &hwm, const HighWaterMarkOptions &options) {
  std::string dbfFilename =
      Adcirc::FileIO::Generic::getFileWithoutExtension(options.output()) +
      ".dbf";
  SHPHandle shp = SHPCreate(options.output().c_str(), SHPT_POINT);
  DBFHandle dbf = DBFCreate(dbfFilename.c_str());

  DBFAddField(dbf, "longitude", FTDouble, 16, 5);
  DBFAddField(dbf, "latitude", FTDouble, 16, 5);

  if (options.outUnits()) {
    DBFAddField(dbf, "observed_ft", FTDouble, 16, 5);
    DBFAddField(dbf, "modeled_ft", FTDouble, 16, 5);
    DBFAddField(dbf, "difference_ft", FTDouble, 16, 5);
    DBFAddField(dbf, "movedist_ft", FTDouble, 16, 5);
  } else {
    DBFAddField(dbf, "longitude", FTDouble, 16, 5);
    DBFAddField(dbf, "latitude", FTDouble, 16, 5);
    DBFAddField(dbf, "observed", FTDouble, 16, 5);
    DBFAddField(dbf, "modeled", FTDouble, 16, 5);
    DBFAddField(dbf, "difference", FTDouble, 16, 5);
    DBFAddField(dbf, "movedist_m", FTDouble, 16, 5);
  }

  double multiplier = options.outConversion();

  for (size_t i = 0; i < hwm.size(); ++i) {
    double x = hwm.location(i)->x();
    double y = hwm.location(i)->y();
    double obs = hwm.location(i)->measured();
    double mod = hwm.location(i)->modeled();
    double diff = hwm.location(i)->difference();
    double movedist = hwm.location(i)->movedDist();
    SHPObject *obj = SHPCreateSimpleObject(SHPT_POINT, 1, &x, &y, nullptr);
    SHPWriteObject(shp, -1, obj);
    SHPDestroyObject(obj);
    DBFWriteDoubleAttribute(dbf, i, 0, x);
    DBFWriteDoubleAttribute(dbf, i, 1, y);
    DBFWriteDoubleAttribute(dbf, i, 2, obs * multiplier);
    DBFWriteDoubleAttribute(dbf, i, 3, mod * multiplier);
    DBFWriteDoubleAttribute(dbf, i, 4, diff * multiplier);
    DBFWriteDoubleAttribute(dbf, i, 5, movedist * multiplier);
  }
  SHPClose(shp);
  DBFClose(dbf);

  return;
}

void writeCsv(Locations &hwm, const HighWaterMarkOptions &options) {
  std::ofstream f;
  f.open(options.output());

  for (size_t i = 0; i < hwm.size(); ++i) {
    f << boost::str(boost::format("%f,%f,0.0,%f,%f,%f,%f") %
                    hwm.location(i)->x() % hwm.location(i)->y() %
                    hwm.location(i)->measured() % hwm.location(i)->modeled() %
                    hwm.location(i)->difference() %
                    hwm.location(i)->movedDist())
      << std::endl;
  }
  f.close();

  return;
}
