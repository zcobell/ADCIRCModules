#ifndef HARMONICSOUTPUT_H
#define HARMONICSOUTPUT_H

#include <string>
#include "harmonicsrecord.h"

class HarmonicsOutputImpl;

namespace Adcirc {
namespace Harmonics {

class HarmonicsOutput {
 public:
  HarmonicsOutput(const std::string& filename = std::string(),
                  bool velocity = false);

  ~HarmonicsOutput();

  void read();

  void write(const std::string& filename);

  std::string filename() const;
  void setFilename(const std::string& filename);

  size_t index(const std::string& name);

  std::string name(size_t index);

  //...fort.53 type parameters
  Adcirc::Harmonics::HarmonicsRecord* amplitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* amplitude(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* phase(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* phase(size_t index);

  //...fort.54 type parameters
  Adcirc::Harmonics::HarmonicsRecord* u_magnitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* u_magnitude(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* u_phase(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* u_phase(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* v_magnitude(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* v_magnitude(size_t index);

  Adcirc::Harmonics::HarmonicsRecord* v_phase(const std::string& name);
  Adcirc::Harmonics::HarmonicsRecord* v_phase(size_t index);

  size_t numConstituents() const;
  void setNumConstituents(const size_t& numConstituents);

  size_t numNodes() const;
  void setNumNodes(const size_t& numNodes);

  bool isVelocity() const;

  size_t nodeIdToArrayIndex(size_t id);

  int filetype() const;

 private:
  HarmonicsOutputImpl* m_impl;
};
}  // namespace Harmonics
}  // namespace Adcirc

#endif  // HARMONICSOUTPUT_H
