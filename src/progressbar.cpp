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
#include "progressbar.h"
#include "boost/progress.hpp"
#include "indicators.hpp"

#ifndef WIN32
#include <unistd.h>
#endif

ProgressBar::ProgressBar(size_t total_iterations, const std::string &prefix)
    : m_display_mode(istty()),
      m_total_iterations(total_iterations),
      m_current(0),
      m_prefix(prefix) {}

ProgressBar::~ProgressBar() = default;

bool ProgressBar::istty() {
#ifdef WIN32
  return false;
#else
  return isatty(STDOUT_FILENO);
#endif
}

void ProgressBar::begin() {
  std::lock_guard<std::mutex> guard(m_mutex);
  if (m_display_mode == 1) {
    indicators::show_console_cursor(false);
    m_indicators_progress = std::make_unique<indicators::ProgressBar>();
    m_indicators_progress->set_option(indicators::option::BarWidth{30});
    m_indicators_progress->set_option(indicators::option::Start{"["});
    m_indicators_progress->set_option(indicators::option::Fill{"="});
    m_indicators_progress->set_option(indicators::option::Lead{">"});
    m_indicators_progress->set_option(indicators::option::End{"]"});
    m_indicators_progress->set_option(indicators::option::Remainder{" "});
    m_indicators_progress->set_option(indicators::option::ShowPercentage(true));
    m_indicators_progress->set_option(
        indicators::option::MaxProgress(m_total_iterations));
    m_indicators_progress->set_option(
        indicators::option::ShowElapsedTime(true));
    m_indicators_progress->set_option(
        indicators::option::ShowRemainingTime(true));
    m_indicators_progress->set_option(indicators::option::PrefixText(m_prefix));
    m_current = 0;
  } else {
    m_boost_progress =
        std::make_unique<boost::progress_display>(m_total_iterations);
  }
}

void ProgressBar::end() {
  if (m_display_mode == 1) {
    indicators::show_console_cursor(true);
    m_indicators_progress.reset(nullptr);
  } else {
    m_boost_progress.reset(nullptr);
  }
}

void ProgressBar::tick() {
  std::lock_guard<std::mutex> guard(m_mutex);
  if (m_display_mode == 1) {
    m_current++;
    if (m_current < m_total_iterations) {
      std::string postfix_string =
          std::to_string(m_current) + "/" + std::to_string(m_total_iterations);
      this->m_indicators_progress->set_option(
          indicators::option::PostfixText(postfix_string));
    } else {
      this->m_indicators_progress->set_option(
          indicators::option::PostfixText(""));
      m_indicators_progress->set_option(
          indicators::option::ShowRemainingTime(false));
    }
    m_indicators_progress->tick();
  } else {
    ++(*m_boost_progress);
  }
}
