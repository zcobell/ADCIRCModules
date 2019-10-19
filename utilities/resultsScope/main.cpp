#include <algorithm>
#include <iostream>
#include <memory>
#include <set>
#include "adcircmodules.h"
#include "boost/format.hpp"
#include "boost/progress.hpp"
#include "cxxopts.hpp"

struct _inputOptions {
  std::string mesh;
  std::string global;
  std::string subdomain;
  std::string output;
  std::string outputMesh;
  _inputOptions()
      : mesh(std::string()),
        global(std::string()),
        subdomain(std::string()),
        output(std::string()),
        outputMesh(std::string()) {}
};

void parseInputOptions(const cxxopts::ParseResult &parser,
                       _inputOptions &options);

int checkInputOptions(const _inputOptions &input);

int generateSubdomain(const std::string &meshFile,
                      const std::string &subdomainFile,
                      const std::string &subdomainMeshFile,
                      std::vector<size_t> &nodeMap,
                      Adcirc::Geometry::Mesh &subdomainMesh);

int generateSubdomainOutput(Adcirc::Geometry::Mesh &subdomainMesh,
                            std::vector<size_t> translation_table,
                            const std::string &globalOutputFile,
                            const std::string &subdomainOutputFile);

Adcirc::Output::OutputRecord *subsetRecord(
    std::vector<size_t> table, Adcirc::Output::OutputRecord *global);

int main(int argc, char *argv[]) {
  cxxopts::Options options("resultScope", "Trim ADCIRC output files to domain");

  // clang-format off
  options.add_options() ("h,help", "show this message")
                     ("mesh", "specify the ADCIRC geometry used in the global file",cxxopts::value<std::string>())
                     ("global","specify the global ADCIRC output",cxxopts::value<std::string>())
                     ("subdomain", "specify the template mesh containing the subdomain",cxxopts::value<std::string>())
                     ("output", "specify the output file to be created",cxxopts::value<std::string>())
                     ("outputMesh", "specify the output mesh file to be created",cxxopts::value<std::string>());
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

  _inputOptions input;
  parseInputOptions(result, input);
  if (checkInputOptions(input) != 0) {
    return 1;
  }

  Adcirc::Geometry::Mesh subdomain;
  std::vector<size_t> translation;
  int ierr = generateSubdomain(input.mesh, input.subdomain, input.outputMesh,
                               translation, subdomain);
  if (ierr != 0) {
    Adcirc::Logging::logError("Could not generate subdomain.", "[ERROR]: ");
    return 1;
  }

  ierr = generateSubdomainOutput(subdomain, translation, input.global,
                                 input.output);

  return 0;
}

void parseInputOptions(const cxxopts::ParseResult &parser,
                       _inputOptions &options) {
  if (parser["mesh"].count() > 0)
    options.mesh = parser["mesh"].as<std::string>();
  if (parser["global"].count() > 0)
    options.global = parser["global"].as<std::string>();
  if (parser["output"].count() > 0)
    options.output = parser["output"].as<std::string>();
  if (parser["subdomain"].count() > 0)
    options.subdomain = parser["subdomain"].as<std::string>();
  if (parser["outputMesh"].count() > 0)
    options.outputMesh = parser["outputMesh"].as<std::string>();
  return;
}

int checkInputOptions(const _inputOptions &input) {
  if (input.mesh == std::string()) {
    Adcirc::Logging::logError("No mesh provided", "[ERROR]: ");
    return 1;
  }
  if (input.global == std::string()) {
    Adcirc::Logging::logError("No global data provided", "[ERROR]: ");
    return 1;
  }
  if (input.output == std::string()) {
    Adcirc::Logging::logError("No output filename provided", "[ERROR]: ");
    return 1;
  }
  if (input.subdomain == std::string()) {
    Adcirc::Logging::logError("No subdomain template provided", "[ERROR]: ");
    return 1;
  }
  if (input.outputMesh == std::string()) {
    Adcirc::Logging::logError("No output mesh name provided", "[ERROR]: ");
    return 1;
  }
  if (!Adcirc::FileIO::Generic::fileExists(input.mesh)) {
    Adcirc::Logging::logError("Mesh file does not exist.");
    return 1;
  }
  if (!Adcirc::FileIO::Generic::fileExists(input.global)) {
    Adcirc::Logging::logError("Global data file does not exist.");
    return 1;
  }
  if (!Adcirc::FileIO::Generic::fileExists(input.subdomain)) {
    Adcirc::Logging::logError("Subdomain mesh file does not exist.");
    return 1;
  }
  return 0;
}

int generateSubdomain(const std::string &meshFile,
                      const std::string &subdomainFile,
                      const std::string &subdomainMeshFile,
                      std::vector<size_t> &nodeMap,
                      Adcirc::Geometry::Mesh &subdomainMesh) {
  Adcirc::Geometry::Mesh globalMesh(meshFile);
  Adcirc::Geometry::Mesh subdomainTemplateMesh(subdomainFile);

  Adcirc::Logging::log("Reading mesh data", "[INFO]: ");
  globalMesh.read();
  subdomainTemplateMesh.read();

  Adcirc::Logging::log("Selecting area of mesh inside subdomain", "[INFO]: ");
  std::vector<int> nodeInside(globalMesh.numNodes());
  std::vector<int> elementInside(globalMesh.numElements());

  std::fill(nodeInside.begin(), nodeInside.end(), 0);
  std::fill(elementInside.begin(), elementInside.end(), 0);

  subdomainTemplateMesh.buildElementalSearchTree();
  for (size_t i = 0; i < globalMesh.numElements(); ++i) {
    Adcirc::Geometry::Element *e = globalMesh.element(i);
    double x, y;
    e->getElementCenter(x, y);
    if (subdomainTemplateMesh.findElement(x, y) !=
        Adcirc::Geometry::Mesh::ELEMENT_NOT_FOUND) {
      elementInside[i] = 1;
      size_t n1 = globalMesh.nodeIndexById(e->node(0)->id());
      size_t n2 = globalMesh.nodeIndexById(e->node(1)->id());
      size_t n3 = globalMesh.nodeIndexById(e->node(2)->id());
      nodeInside[n1] = 1;
      nodeInside[n2] = 1;
      nodeInside[n3] = 1;
    }
  }

  size_t nn = std::accumulate(nodeInside.begin(), nodeInside.end(), 0);
  size_t ne = std::accumulate(elementInside.begin(), elementInside.end(), 0);
  Adcirc::Logging::log(
      boost::str(boost::format("Found %i nodes and %i elements in subdomain") %
                 nn % ne),
      "[INFO]: ");

  Adcirc::Logging::log("Building subdomain mesh", "[INFO]: ");
  nodeMap.reserve(nn);
  subdomainMesh.setMeshHeaderString(globalMesh.meshHeaderString());
  subdomainMesh.resizeMesh(nn, ne, 0, 0);
  std::vector<Adcirc::Geometry::Node *> nodeListVector;
  nodeListVector.reserve(nn * 10);
  for (size_t i = 0; i < globalMesh.numElements(); ++i) {
    if (elementInside[i] == 1) {
      Adcirc::Geometry::Node *n1 = globalMesh.element(i)->node(0);
      Adcirc::Geometry::Node *n2 = globalMesh.element(i)->node(1);
      Adcirc::Geometry::Node *n3 = globalMesh.element(i)->node(2);
      nodeListVector.push_back(n1);
      nodeListVector.push_back(n2);
      nodeListVector.push_back(n3);
    }
  }

  std::set<Adcirc::Geometry::Node *> nodeList(nodeListVector.begin(),
                                              nodeListVector.end());
  nodeListVector.clear();
  nodeListVector =
      std::vector<Adcirc::Geometry::Node *>(nodeList.begin(), nodeList.end());
  nodeList.clear();

  std::unordered_map<size_t, size_t> revNodeMap;
  revNodeMap.reserve(nn);

  for (size_t i = 0; i < nodeListVector.size(); ++i) {
    revNodeMap[nodeListVector[i]->id() - 1] = i;
    nodeMap.push_back(nodeListVector[i]->id() - 1);
    subdomainMesh.addNode(i, nodeListVector[i]);
  }
  nodeListVector.clear();

  size_t counter = 0;
  for (size_t i = 0; i < globalMesh.numElements(); ++i) {
    if (elementInside[i] == 1) {
      Adcirc::Geometry::Element *e = globalMesh.element(i);
      Adcirc::Geometry::Node *n1 =
          subdomainMesh.node(revNodeMap[e->node(0)->id() - 1]);
      Adcirc::Geometry::Node *n2 =
          subdomainMesh.node(revNodeMap[e->node(1)->id() - 1]);
      Adcirc::Geometry::Node *n3 =
          subdomainMesh.node(revNodeMap[e->node(2)->id() - 1]);
      subdomainMesh.addElement(
          counter, Adcirc::Geometry::Element(counter + 1, n1, n2, n3));
      counter++;
    }
  }

  Adcirc::Logging::log("Renumbering subdomain mesh", "[INFO]: ");
  for (size_t i = 0; i < subdomainMesh.numNodes(); ++i) {
    subdomainMesh.node(i)->setId(i + 1);
  }

  Adcirc::Logging::log("Writing subdomain mesh file", "[INFO]: ");
  subdomainMesh.write(subdomainMeshFile);

  return 0;
}

int generateSubdomainOutput(Adcirc::Geometry::Mesh &subdomainMesh,
                            std::vector<size_t> translation_table,
                            const std::string &globalOutputFile,
                            const std::string &subdomainOutputFile) {
  Adcirc::Logging::log("Writing subdomain output data", "[INFO]: ");

  Adcirc::Output::ReadOutput global(globalOutputFile);
  global.open();

  Adcirc::Output::WriteOutput out(subdomainOutputFile, &global, &subdomainMesh);
  out.open();

  std::unique_ptr<boost::progress_display> progress(
      new boost::progress_display(global.numSnaps()));

  for (size_t i = 0; i < global.numSnaps(); ++i) {
    ++(*progress.get());
    global.read(i);
    std::unique_ptr<Adcirc::Output::OutputRecord> r(
        subsetRecord(translation_table, global.dataAt(0)));
    out.write(r.get());
    global.clearAt(0);
  }

  global.close();
  out.close();

  return 0;
}

Adcirc::Output::OutputRecord *subsetRecord(
    std::vector<size_t> table, Adcirc::Output::OutputRecord *global) {
  size_t nn = table.size();
  Adcirc::Output::OutputRecord *r = new Adcirc::Output::OutputRecord(
      global->record(), nn, *global->metadata());
  for (size_t i = 0; i < nn; ++i) {
    if (r->metadata()->isVector()) {
      r->setU(i, global->u(table[i]));
      r->setV(i, global->v(table[i]));
    } else {
      r->set(i, global->z(table[i]));
    }
  }
  return r;
}
