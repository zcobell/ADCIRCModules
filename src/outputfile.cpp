#include "outputfile.h"
#include <fstream>
#include "filetypes.h"
#include "logging.h"
#include "outputrecord.h"

using namespace Adcirc::Output;

OutputFile::OutputFile(const std::string& filename)
    : m_filename(filename),
      m_currentSnap(0),
      m_numNodes(0),
      m_numSnaps(0),
      m_dt(0),
      m_dit(0),
      m_open(false),
      m_isVector(false),
      m_isMax(false),
      m_defaultValue(Adcirc::Output::defaultOutputValue()),
      m_filetype(Adcirc::Output::OutputUnknown),
      m_units("n/a"),
      m_description("n/a"),
      m_name("n/a") {}

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

void OutputFile::setNumSnaps(int numSnaps) { this->m_numSnaps = numSnaps; }

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

std::string OutputFile::name() const { return this->m_name; }

Adcirc::Output::OutputFormat OutputFile::setFiletype(OutputFormat filetype) {
  this->m_filetype = filetype;
}

size_t OutputFile::currentSnap() const { return m_currentSnap; }

void OutputFile::setCurrentSnap(const size_t& currentSnap) {
  m_currentSnap = currentSnap;
}

std::string OutputFile::description() const { return this->m_description; }

std::string OutputFile::units() const { return this->m_units; }

void OutputFile::setOpen(bool open) { this->m_open = open; }

bool OutputFile::isMax() const
{
  return m_isMax;
}

double OutputFile::defaultValue() const
{
  return m_defaultValue;
}

void OutputFile::setDefaultValue(double defaultValue)
{
  m_defaultValue = defaultValue;
}

bool OutputFile::isVector() const
{
  return m_isVector;
}

void OutputFile::setIsVector(bool isVector)
{
  m_isVector = isVector;
}

void OutputFile::setIsMax(bool isMax)
{
  m_isMax = isMax;
}

void OutputFile::setUnits(const std::string &units)
{
  m_units = units;
}

void OutputFile::setDescription(const std::string &description)
{
  m_description = description;
}

void OutputFile::setName(const std::string& name) { this->m_name = name; }
