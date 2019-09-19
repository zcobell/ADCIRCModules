#include "outputmetadata.h"
#include "logging.h"

using namespace Adcirc::Output;

OutputMetadata::OutputMetadata() : m_isMax(false), m_isVector(false) {
  this->setDimension(1);
}

OutputMetadata::OutputMetadata(const std::string& name,
                               const std::string& longName,
                               const std::string& standardName,
                               const std::string& units,
                               const std::string& convention, bool isMaximum)
    : m_isMax(isMaximum),
      m_isVector(false),
      m_dimensions(1),
      m_variable({name}),
      m_longName({longName}),
      m_standardName({standardName}),
      m_units({units}),
      m_convention({convention}) {}

OutputMetadata::OutputMetadata(
    const std::string& name1, const std::string& longName1,
    const std::string& standardName1, const std::string& units1,
    const std::string& convention1, const std::string& name2,
    const std::string& longName2, const std::string& standardName2,
    const std::string& units2, const std::string& convention2, bool isMaximum,
    bool isVector)
    : m_isMax(isMaximum),
      m_isVector(isVector),
      m_dimensions(2),
      m_variable({name1, name2}),
      m_longName({longName1, longName2}),
      m_standardName({standardName1, standardName2}),
      m_units({units1, units2}),
      m_convention({convention1, convention2}) {}

OutputMetadata::OutputMetadata(
    const std::string& name1, const std::string& longName1,
    const std::string& standardName1, const std::string& units1,
    const std::string& convention1, const std::string& name2,
    const std::string& longName2, const std::string& standardName2,
    const std::string& units2, const std::string& convention2,
    const std::string& name3, const std::string& longName3,
    const std::string& standardName3, const std::string& units3,
    const std::string& convention3, bool isMaximum, bool isVector)
    : m_isMax(isMaximum),
      m_isVector(isVector),
      m_dimensions(3),
      m_variable({name1, name2, name3}),
      m_longName({longName1, longName2, longName3}),
      m_standardName({standardName1, standardName2, standardName3}),
      m_units({units1, units2, units3}),
      m_convention({convention1, convention2, convention3}) {}

bool OutputMetadata::contains(const std::string& name) {
  for (auto& v : this->m_variable) {
    if (v == name) return true;
  }
  return false;
}

bool OutputMetadata::operator==(const OutputMetadata& rhs) {
  if (this->dimension() != rhs.dimension()) return false;
  for (size_t i = 0; i < this->dimension(); ++i) {
    if (this->variable(i) != rhs.variable(i)) return false;
  }
  return true;
}

bool OutputMetadata::isMax() const { return this->m_isMax; }

void OutputMetadata::setDimension(size_t size) {
  if (size > 3)
    adcircmodules_throw_exception(
        "OutputMetadata: Dimension must be less than 3");

  this->m_variable.resize(size);
  this->m_standardName.resize(size);
  this->m_longName.resize(size);
  this->m_convention.resize(size);
  this->m_units.resize(size);
  this->m_dimensions = size;
}

size_t OutputMetadata::dimension() const { return this->m_dimensions; }

bool OutputMetadata::isVector() const { return this->m_isVector; }

void OutputMetadata::setIsVector(const bool isVector) {
  this->m_isVector = isVector;
}

void OutputMetadata::setIsMax(const bool isMax) { this->m_isMax = isMax; }

void OutputMetadata::setVariable(size_t index, const std::string& name) {
  if (index < this->m_variable.size()) {
    this->m_variable[index] = name;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

std::string OutputMetadata::variable(size_t index) const {
  if (index < this->m_variable.size()) return this->m_variable[index];
  std::cout << index << " " << this->m_variable.size() << " "
            << this->m_dimensions << std::endl;
  std::cout.flush();
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

void OutputMetadata::setLongName(size_t index, const std::string& longName) {
  if (index < this->m_longName.size()) {
    this->m_longName[index] = longName;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

std::string OutputMetadata::longName(size_t index) const {
  if (index < this->m_longName.size()) return this->m_longName[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

void OutputMetadata::setStandardName(size_t index,
                                     const std::string& standardName) {
  if (index < this->m_standardName.size()) {
    this->m_standardName[index] = standardName;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

std::string OutputMetadata::standardName(size_t index) const {
  if (index < this->m_standardName.size()) return this->m_standardName[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

void OutputMetadata::setUnits(size_t index, const std::string& units) {
  if (index < this->m_units.size()) {
    this->m_units[index] = units;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

std::string OutputMetadata::units(size_t index) const {
  if (index < this->m_units.size()) return this->m_units[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

void OutputMetadata::setConvention(size_t index,
                                   const std::string& convention) {
  if (index < this->m_convention.size()) {
    this->m_convention[index] = convention;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

std::string OutputMetadata::convention(size_t index) const {
  if (index < this->m_convention.size()) return this->m_convention[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}
