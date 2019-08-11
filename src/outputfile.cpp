#include "outputfile.h"
#include <fstream>
#include "adcirc_outputfiles.h"
#include "filetypes.h"
#include "logging.h"
#include "outputrecord.h"

using namespace Adcirc::Output;

const std::vector<OutputMetadata>* OutputFile::adcircFileMetadata() {
  return &c_outputMetadata;
}

OutputFile::OutputFile(const std::string& filename)
    : m_filename(filename),
      m_currentSnap(0),
      m_numNodes(0),
      m_numSnaps(0),
      m_dt(0),
      m_modelDt(0),
      m_dit(0),
      m_open(false),
      m_defaultValue(Adcirc::Output::defaultOutputValue()),
      m_filetype(Adcirc::Output::OutputUnknown),
      m_units("n/a"),
      m_description("n/a"),
      m_name("n/a"),
      m_metadata(OutputMetadata()) {}

OutputFile::~OutputFile() {}

std::string OutputFile::filename() const { return this->m_filename; }

void OutputFile::setFilename(const std::string& filename) {
  if (!this->isOpen()) {
    this->m_filename = filename;
  } else {
    adcircmodules_throw_exception(
        "OutputFile: Cannot change filename since file currently open");
  }
}

bool OutputFile::isOpen() { return this->m_open; }

bool OutputFile::exists() {
  std::ifstream f(this->m_filename.c_str());
  return f.good();
}

size_t OutputFile::numSnaps() const { return this->m_numSnaps; }

void OutputFile::setNumSnaps(size_t numSnaps) { this->m_numSnaps = numSnaps; }

size_t OutputFile::numNodes() const { return this->m_numNodes; }

void OutputFile::setNumNodes(size_t numNodes) { this->m_numNodes = numNodes; }

double OutputFile::dt() const { return this->m_dt; }

void OutputFile::setDt(double dt) { this->m_dt = dt; }

int OutputFile::dIteration() const { return this->m_dit; }

void OutputFile::setDiteration(int dit) { this->m_dit = dit; }

OutputFormat OutputFile::filetype() const { return this->m_filetype; }

std::string OutputFile::header() const { return this->m_header; }

void OutputFile::setHeader(const std::string& header) {
  this->m_header = header;
}

Adcirc::Output::OutputFormat OutputFile::setFiletype(OutputFormat filetype) {
  this->m_filetype = filetype;
}

size_t OutputFile::currentSnap() const { return this->m_currentSnap; }

void OutputFile::setCurrentSnap(const size_t& currentSnap) {
  this->m_currentSnap = currentSnap;
}

void OutputFile::setOpen(bool open) { this->m_open = open; }

double OutputFile::modelDt() const { return this->m_modelDt; }

void OutputFile::setModelDt(double modelDt) { this->m_modelDt = modelDt; }

OutputMetadata* OutputFile::metadata() { return &this->m_metadata; }

void OutputFile::setMetadata(const OutputMetadata& metadata) {
  this->m_metadata = metadata;
}

double OutputFile::defaultValue() const { return this->m_defaultValue; }

void OutputFile::setDefaultValue(double defaultValue) {
  this->m_defaultValue = defaultValue;
}
