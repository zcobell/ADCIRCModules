#ifndef INTERPOLATER_H
#define INTERPOLATER_H

#include <string>
#include "adcircmodules.h"

class Interpolate {
 public:
  struct InputOptions {
    std::string mesh, globalfile, stationfile, outputfile, coldstart, refdate;
    bool magnitude, direction, readimeds, writeimeds, readdflow, writedflow,
        readasciimesh, keepwet;
    int startsnap, endsnap, epsg_global, epsg_station, epsg_output;
    double multiplier;
  };

  Interpolate(const InputOptions &options);

  void run();

 private:
  InputOptions m_inputOptions;
};

#endif  // INTERPOLATER_H
