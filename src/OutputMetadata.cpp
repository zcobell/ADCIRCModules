#include "OutputMetadata.h"

#include "Logging.h"

using namespace Adcirc::Output;

/**
 * @brief Default constructor
 */
OutputMetadata::OutputMetadata() : m_isMax(false), m_isVector(false) {
  this->setDimension(1);
}

/**
 * @brief Constructor for a 1d dataset
 * @param[in] name name for attribute
 * @param[in] longName full name for attribute
 * @param[in] standardName standard name for attribute
 * @param[in] units units for attribute
 * @param[in] convention convention for attribute
 * @param[in] isMaximum true if this is a maximum file
 */
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

/**
 * @brief Constructor for a 2d dataset
 * @param[in] name1 name for first attribute
 * @param[in] longName1 full name for first attribute
 * @param[in] standardName1 standard name for first attribute
 * @param[in] units1 units for first attribute
 * @param[in] convention1 convention for first attribute
 * @param[in] name2 name for second attribute
 * @param[in] longName2 full name for second attribute
 * @param[in] standardName2 standard name for second attribute
 * @param[in] units2 units for second attribute
 * @param[in] convention2 convention for second attribute
 * @param[in] name3 name for third attribute
 * @param[in] isMaximum true if this is a maximum file
 * @param[in] isVector true if this is a vector file
 */
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

/**
 * @brief Constructor for a 3d dataset
 * @param[in] name1 name for first attribute
 * @param[in] longName1 full name for first attribute
 * @param[in] standardName1 standard name for first attribute
 * @param[in] units1 units for first attribute
 * @param[in] convention1 convention for first attribute
 * @param[in] name2 name for second attribute
 * @param[in] longName2 full name for second attribute
 * @param[in] standardName2 standard name for second attribute
 * @param[in] units2 units for second attribute
 * @param[in] convention2 convention for second attribute
 * @param[in] name3 name for third attribute
 * @param[in] longName3 long name for third attribute
 * @param[in] standardName3 standard name for third attribute
 * @param[in] units3 units for third attribute
 * @param[in] convention3 convention for third attribute
 * @param[in] isMaximum true if this is a maximum file
 * @param[in] isVector true if this is a vector file
 */
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

/**
 * @brief Returns true if the specified variable is found in this object
 * @param name variable to attempt to find
 * @return true if found
 */
bool OutputMetadata::contains(const std::string& name) {
  for (auto& v : this->m_variable) {
    if (v == name) return true;
  }
  return false;
}

/**
 * @brief Checks for equality between OutputMetadata objects
 * @param rhs comparison object
 * @return true if variables contained are the same
 */
bool OutputMetadata::operator==(const OutputMetadata& rhs) {
  if (this->dimension() != rhs.dimension()) return false;
  for (size_t i = 0; i < this->dimension(); ++i) {
    if (this->variable(i) != rhs.variable(i)) return false;
  }
  return true;
}

/**
 * @brief Checks if the object represents a maximum
 * @return true if maximum
 */
bool OutputMetadata::isMax() const { return this->m_isMax; }

/**
 * @brief Sets the dimension size for the object
 * @param size dimension size
 */
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

/**
 * @brief Returns the dimension of the object
 * @return size (dimension) of object
 */
size_t OutputMetadata::dimension() const { return this->m_dimensions; }

/**
 * @brief Returns true if this represents a vector object
 * @return true if vector
 */
bool OutputMetadata::isVector() const { return this->m_isVector; }

/**
 * @brief Sets the object to represent a vector (if true)
 * @param[in] isVector boolean representing vector state
 */
void OutputMetadata::setIsVector(const bool isVector) {
  this->m_isVector = isVector;
}

/**
 * @brief Sets the object to represent a maximum (if true)
 * @param[in] isMax boolean representing maximum file state
 */
void OutputMetadata::setIsMax(const bool isMax) { this->m_isMax = isMax; }

/**
 * @brief Sets the variable name at the specified index
 * @param[in] index variable position to alter
 * @param[in] name name of variable to place at the position
 */
void OutputMetadata::setVariable(size_t index, const std::string& name) {
  if (index < this->m_variable.size()) {
    this->m_variable[index] = name;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

/**
 * @brief Returns the variable at the specified index
 * @param[in] index index of variable to get
 * @return variable name
 */
std::string OutputMetadata::variable(size_t index) const {
  if (index < this->m_variable.size()) return this->m_variable[index];
  std::cout << index << " " << this->m_variable.size() << " "
            << this->m_dimensions << std::endl;
  std::cout.flush();
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

/**
 * @brief Sets the long name for the variable at the specified index
 * @param[in] index index to set long name for
 * @param[in] longName long name of variable
 */
void OutputMetadata::setLongName(size_t index, const std::string& longName) {
  if (index < this->m_longName.size()) {
    this->m_longName[index] = longName;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

/**
 * @brief Returns long name at specified position
 * @param[in] index index to return long name for
 * @return long name of object at index
 */
std::string OutputMetadata::longName(size_t index) const {
  if (index < this->m_longName.size()) return this->m_longName[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

/**
 * @brief Sets the standard name for the variable at the specified index
 * @param[in] index index to set standard name for
 * @param[in] standardName standard name of variable
 */
void OutputMetadata::setStandardName(size_t index,
                                     const std::string& standardName) {
  if (index < this->m_standardName.size()) {
    this->m_standardName[index] = standardName;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

/**
 * @brief Returns standard name at specified position
 * @param[in] index index to return standard name for
 * @return standard name of object at index
 */
std::string OutputMetadata::standardName(size_t index) const {
  if (index < this->m_standardName.size()) return this->m_standardName[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

/**
 * @brief Sets the units for the variable at the specified index
 * @param[in] index index to set units for
 * @param[in] units units of variable
 */
void OutputMetadata::setUnits(size_t index, const std::string& units) {
  if (index < this->m_units.size()) {
    this->m_units[index] = units;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

/**
 * @brief Returns units at specified position
 * @param[in] index index to return units for
 * @return units of object at index
 */
std::string OutputMetadata::units(size_t index) const {
  if (index < this->m_units.size()) return this->m_units[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}

/**
 * @brief Sets the convention for the variable at the specified index
 * @param[in] index index to set convention for
 * @param[in] convention convention of variable
 */
void OutputMetadata::setConvention(size_t index,
                                   const std::string& convention) {
  if (index < this->m_convention.size()) {
    this->m_convention[index] = convention;
  } else {
    adcircmodules_throw_exception("OutputMetadata: Index out of range");
  }
}

/**
 * @brief Returns convention at specified position
 * @param[in] index index to return convention for
 * @return convention of object at index
 */
std::string OutputMetadata::convention(size_t index) const {
  if (index < this->m_convention.size()) return this->m_convention[index];
  adcircmodules_throw_exception("OutputMetadata: Index out of range.");
  return std::string();
}
