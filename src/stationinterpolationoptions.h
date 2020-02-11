#ifndef ADCMOD_STATIONINTERPOLATIONOPTIONS_H
#define ADCMOD_STATIONINTERPOLATIONOPTIONS_H

#include <string>
#include <vector>

#include "adcircmodules_global.h"
#include "hmdf.h"

namespace Adcirc {
namespace Output {

class StationInterpolationOptions {
 public:
  ADCIRCMODULES_EXPORT StationInterpolationOptions();

  std::string ADCIRCMODULES_EXPORT mesh() const;
  void ADCIRCMODULES_EXPORT setMesh(const std::string &mesh);

  std::string ADCIRCMODULES_EXPORT globalfile() const;
  void ADCIRCMODULES_EXPORT setGlobalfile(const std::string &globalfile);

  std::string ADCIRCMODULES_EXPORT stationfile() const;
  void ADCIRCMODULES_EXPORT setStationfile(const std::string &stationfile);

  std::string ADCIRCMODULES_EXPORT outputfile() const;
  void ADCIRCMODULES_EXPORT setOutputfile(const std::string &outputfile);

  std::string ADCIRCMODULES_EXPORT coldstart() const;
  void ADCIRCMODULES_EXPORT setColdstart(const std::string &coldstart);

  std::string ADCIRCMODULES_EXPORT refdate() const;
  void ADCIRCMODULES_EXPORT setRefdate(const std::string &refdate);

  bool ADCIRCMODULES_EXPORT magnitude() const;
  void ADCIRCMODULES_EXPORT setMagnitude(bool magnitude);

  bool ADCIRCMODULES_EXPORT direction() const;
  void ADCIRCMODULES_EXPORT setDirection(bool direction);

  bool ADCIRCMODULES_EXPORT readasciimesh() const;
  void ADCIRCMODULES_EXPORT setReadasciimesh(bool readasciimesh);

  size_t ADCIRCMODULES_EXPORT startsnap() const;
  void ADCIRCMODULES_EXPORT setStartsnap(const size_t &startsnap);

  size_t ADCIRCMODULES_EXPORT endsnap() const;
  void ADCIRCMODULES_EXPORT setEndsnap(const size_t &endsnap);

  int ADCIRCMODULES_EXPORT epsgGlobal() const;
  void ADCIRCMODULES_EXPORT setEpsgGlobal(int epsgGlobal);

  int ADCIRCMODULES_EXPORT epsgStation() const;
  void ADCIRCMODULES_EXPORT setEpsgStation(int epsgStation);

  int ADCIRCMODULES_EXPORT epsgOutput() const;
  void ADCIRCMODULES_EXPORT setEpsgOutput(int epsgOutput);

  double ADCIRCMODULES_EXPORT multiplier() const;
  void ADCIRCMODULES_EXPORT setMultiplier(double multiplier);

  void ADCIRCMODULES_EXPORT readStations(const std::string &stationFile = std::string());
  Adcirc::Output::HmdfStation ADCIRCMODULES_EXPORT *station(size_t index);
  Adcirc::Output::Hmdf ADCIRCMODULES_EXPORT *stations();

  void ADCIRCMODULES_EXPORT usePositiveDirection(bool usePositiveDirection);
  bool ADCIRCMODULES_EXPORT hasPositiveDirection() const;
  void ADCIRCMODULES_EXPORT readPositiveDirection(const std::string &positiveDirections);
  void ADCIRCMODULES_EXPORT setPositiveDirection(size_t index, double positiveDirection);
  void ADCIRCMODULES_EXPORT setPositiveDirections(double positiveDirection);

private:
  Adcirc::Output::Hmdf readStationList(const std::string &station);

  std::string m_mesh;
  std::string m_globalfile;
  std::string m_stationfile;
  std::string m_outputfile;
  std::string m_coldstart;
  std::string m_refdate;

  bool m_magnitude;
  bool m_direction;
  bool m_readasciimesh;
  bool m_hasPositiveDirection;

  size_t m_startsnap;
  size_t m_endsnap;

  int m_epsgGlobal;
  int m_epsgStation;
  int m_epsgOutput;

  double m_multiplier;

  Adcirc::Output::Hmdf m_hmdf;
};
}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_STATIONINTERPOLATIONOPTIONS_H
