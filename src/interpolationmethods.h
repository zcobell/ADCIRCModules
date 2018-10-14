#ifndef INTERPOLATIONMETHODS_H
#define INTERPOLATIONMETHODS_H

namespace Interpolation {
enum Method {
  NoMethod = 0,
  Average = 1,
  Nearest = 2,
  Highest = 3,
  PlusTwoSigma = 4,
  BilskieEtAll = 5,
  InverseDistanceWeighted = 6
};
}

#endif  // INTERPOLATIONMETHODS_H
