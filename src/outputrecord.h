/*------------------------------GPL---------------------------------------//
// This file is part of ADCIRCModules.
//
// (c) 2015-2019 Zachary Cobell
//
// ADCIRCModules is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// ADCIRCModules is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------*/
#ifndef ADCMOD_OUTPUTRECORD_H
#define ADCMOD_OUTPUTRECORD_H

#include <cstdlib>
#include <limits>
#include <vector>
#include "adcircmodules_global.h"
#include "cdate.h"
#include "default_values.h"
#include "node.h"
#include "outputmetadata.h"

namespace Adcirc {

namespace Output {

constexpr size_t nextOutputSnap() {
  return adcircmodules_default_value<size_t>();
}
constexpr double defaultOutputValue() { return -99999.0; }

enum AngleUnits { Degrees, Radians };

/**
 * @class OutputRecord
 * @author Zachary Cobell
 * @copyright Copyright 2015-2019 Zachary Cobell. All Rights Reserved. This
 * project is released under the terms of the GNU General Public License v3
 * @brief The OutputRecord class describes a single output snap from an ADCIRC
 * output file. The data can have 1, 2, or 3 dimensions
 *
 */
class OutputRecord {
 public:
  OutputRecord();
  OutputRecord(const size_t record, const size_t numNodes,
               Adcirc::Output::OutputMetadata& metadata,
               const CDate& coldstart = CDate(1970, 1, 1, 0, 0, 0));
  OutputRecord(const size_t record, const size_t numNodes, const bool isMax,
               const bool isVector, const size_t dimension,
               const CDate& coldstart = CDate(1970, 1, 1, 0, 0, 0));

  void fill(double z);

  void setU(size_t index, double z);
  void setV(size_t index, double z);
  void setW(size_t index, double value);
  void set(size_t index, double z);
  void set(size_t index, double value_u, double value_v);
  void set(size_t index, double value_u, double value_v, double value_w);

  void setAllU(const std::vector<double>& values);
  void setAllV(const std::vector<double>& values);
  void setAllW(const std::vector<double>& values);
  void setAll(const std::vector<double>& values);
  void setAll(const std::vector<double>& values_u,
              const std::vector<double>& values_v);
  void setAll(const std::vector<double>& values_u,
              const std::vector<double>& values_v,
              const std::vector<double>& values_w);
  void setAll(size_t size, const double* values);
  void setAll(size_t size, const double* values_u, const double* values_v);
  void setAll(size_t size, const double* values_u, const double* values_v,
              const double* values_w);

  std::vector<double> values(size_t column = 0);
  std::vector<double> magnitudes();
  std::vector<double> directions(AngleUnits angleType = AngleUnits::Degrees);

  double z(size_t index) const;
  double u(size_t index) const;
  double v(size_t index) const;
  double w(size_t index) const;
  double magnitude(size_t index) const;
  double direction(size_t index,
                   AngleUnits angleType = AngleUnits::Degrees) const;

  long long iteration() const;
  void setIteration(long long iteration);

  double time() const;
  void setTime(double time);

  size_t numNodes() const;
  void setNumNodes(size_t numNodes);

  double defaultValue() const;
  void setDefaultValue(double defaultValue);

  size_t record() const;
  void setRecord(size_t record);

  size_t numNonDefault() const;

  bool isDefault(size_t index) const;

  Adcirc::Output::OutputMetadata* metadata();
  void setMetadata(const Adcirc::Output::OutputMetadata& metadata);

  void setColdstart(const CDate& date);
  Adcirc::CDate coldstart() const;
  Adcirc::CDate date() const;

 private:
  std::vector<double> m_u;
  std::vector<double> m_v;
  std::vector<double> m_w;
  Adcirc::Output::OutputMetadata m_metadata;
  size_t m_record;
  long long m_iteration;
  double m_time;
  size_t m_numNodes;
  double m_defaultValue;
  Adcirc::CDate m_coldstart;
  Adcirc::CDate m_date;

  double angle(double x, double y, AngleUnits units) const;
  void allocate();
};
}  // namespace Output
}  // namespace Adcirc

#endif  // ADCMOD_OUTPUTRECORD_H
