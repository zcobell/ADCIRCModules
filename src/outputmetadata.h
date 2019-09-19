#ifndef ADCMOD_OUTPUTMETADATA_H
#define ADCMOD_OUTPUTMETADATA_H

#include <string>
#include <vector>

namespace Adcirc {
namespace Output {
class OutputMetadata {
 public:
  static const std::string defaultConvention() { return "default"; }

  OutputMetadata();

  OutputMetadata(const std::string& name, const std::string& longName,
                 const std::string& standardName, const std::string& units,
                 const std::string& convention, bool isMaximum = false);

  OutputMetadata(const std::string& name1, const std::string& longName1,
                 const std::string& standardName1, const std::string& units1,
                 const std::string& convention1, const std::string& name2,
                 const std::string& longName2, const std::string& standardName2,
                 const std::string& units2, const std::string& convention2,
                 bool isMaximum = false, bool isVector = false);

  OutputMetadata(const std::string& name1, const std::string& longName1,
                 const std::string& standardName1, const std::string& units1,
                 const std::string& convention1, const std::string& name2,
                 const std::string& longName2, const std::string& standardName2,
                 const std::string& units2, const std::string& convention2,
                 const std::string& name3, const std::string& longName3,
                 const std::string& standardName3, const std::string& units3,
                 const std::string& convention3, bool isMaximum = false,
                 bool isVector = false);

  bool operator==(const OutputMetadata& rhs);

  std::string variable(size_t index = 0) const;

  std::string longName(size_t index = 0) const;

  std::string standardName(size_t index = 0) const;

  std::string units(size_t index = 0) const;

  std::string convention(size_t index = 0) const;

  void setVariable(size_t index, const std::string& name);

  void setLongName(size_t index, const std::string& longName);

  void setStandardName(size_t index, const std::string& standardName);

  void setUnits(size_t index, const std::string& units);

  void setConvention(size_t index, const std::string& convention);

  bool contains(const std::string& variable);

  bool isMax() const;

  void setDimension(size_t size);

  size_t dimension() const;

  bool isVector() const;

  void setIsVector(const bool isVector);

  void setIsMax(const bool isMax);

 private:
  std::vector<std::string> m_variable;
  std::vector<std::string> m_longName;
  std::vector<std::string> m_standardName;
  std::vector<std::string> m_units;
  std::vector<std::string> m_convention;
  bool m_isMax;
  bool m_isVector;
  size_t m_dimensions;
};
}  // namespace Output
}  // namespace Adcirc
#endif  // ADCMOD_OUTPUTMETADATA_H
