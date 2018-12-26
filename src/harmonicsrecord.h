#ifndef HARMONICSRECORD_H
#define HARMONICSRECORD_H

#include <string>
#include <vector>

class HarmonicsRecordImpl;

namespace Adcirc {
namespace Harmonics {
class HarmonicsRecord {
 public:
  HarmonicsRecord();
  ~HarmonicsRecord();

  std::string name() const;
  void setName(const std::string& name);

  double value(size_t index);
  std::vector<double> values();

  void set(size_t index, double value);
  void set(const std::vector<double>& value);

  void resize(size_t numNodes);

  double frequency() const;
  void setFrequency(double frequency);

  double nodalFactor() const;
  void setNodalFactor(double nodalFactor);

  double equilibriumArg() const;
  void setEquilibriumArg(double equilibriumArg);

 private:
  HarmonicsRecordImpl *m_impl;
};
}  // namespace Harmonics
}  // namespace Adcirc

#endif  // HARMONICSRECORD_H
