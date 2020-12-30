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
#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <memory>
#include <mutex>
#include <string>

namespace indicators {
class ProgressBar;
}

namespace boost {
class progress_display;
}

class ProgressBar {
 public:
  ProgressBar(size_t total_iterations,
              const std::string &prefix = std::string());
  ~ProgressBar();

  void setPrefix(const std::string &prefix);
  void begin();
  void tick();
  void end();

 private:
  static bool istty();

  short m_display_mode;
  size_t m_total_iterations;
  unsigned long long m_current;
  std::string m_prefix;
  std::mutex m_mutex;
  std::unique_ptr<indicators::ProgressBar> m_indicators_progress;
  std::unique_ptr<boost::progress_display> m_boost_progress;
};

#endif  // PROGRESSBAR_H
