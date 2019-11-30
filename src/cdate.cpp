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
#include "cdate.h"
#include <chrono>
#include <iostream>
#include "boost/format.hpp"
#include "date.h"
#include "logging.h"

using namespace Adcirc;

struct s_date {
 private:
  date::year_month_day dd;

 public:
  s_date(const std::chrono::system_clock::time_point &t)
      : dd(date::year_month_day(date::floor<date::days>(t))) {}
  int year() const { return int(dd.year()); }
  unsigned month() const { return unsigned(dd.month()); }
  unsigned day() const { return unsigned(dd.day()); }
};

struct s_datetime {
 private:
  date::year_month_day dd;
  date::hh_mm_ss<std::chrono::duration<long int, std::ratio<1, 1000000000>>> tt;

 public:
  s_datetime(const std::chrono::system_clock::time_point &t)
      : dd(date::year_month_day(date::floor<date::days>(t))),
        tt(date::make_time(t - date::sys_days(dd))) {}
  int year() const { return int(dd.year()); }
  unsigned month() const { return unsigned(dd.month()); }
  unsigned day() const { return unsigned(dd.day()); }
  int hour() const { return tt.hours().count(); }
  int minute() const { return tt.minutes().count(); }
  int second() const { return tt.seconds().count(); }
};

constexpr date::year_month_day c_epoch() {
  date::year_month_day(date::year(1970) / 1 / 1);
}

CDate::CDate() { this->set(1970, 1, 1, 0, 0, 0); }

CDate::CDate(const std::chrono::system_clock::time_point &t) { this->set(t); }

CDate::CDate(const std::vector<int> &v) { this->set(v); }

CDate::CDate(const std::array<int, 6> &v) { this->set(v); }

CDate::CDate(const CDate &d) { this->set(d.get()); }

CDate::CDate(int year, int month, int day, int hour, int minute, int second) {
  this->set(year, month, day, hour, minute, second);
}

bool CDate::operator<(const CDate &d) const {
  return this->time_point() < d.time_point();
}

bool CDate::operator==(const CDate &d) const {
  return this->time_point() == d.time_point();
}

bool CDate::operator!=(const CDate &d) const {
  return this->time_point() != d.time_point();
}

void CDate::set(int year, int month, int day, int hour, int minute,
                int second) {
  auto ymd = date::year(year) / date::month(month) / date::day(day);
  if (!ymd.ok()) {
    adcircmodules_throw_exception("Invalid date");
  }
  this->m_datetime = date::sys_days(ymd) + std::chrono::hours(hour) +
                     std::chrono::minutes(minute) +
                     std::chrono::seconds(second);
  return;
}

std::vector<int> CDate::get() const {
  s_datetime time(this->m_datetime);
  std::vector<int> v(6);
  v[0] = time.year();
  v[1] = time.month();
  v[2] = time.day();
  v[3] = time.hour();
  v[4] = time.minute();
  v[5] = time.second();
  return v;
}

void CDate::set(const std::vector<int> &v) {
  if (v.size() != 6)
    adcircmodules_throw_exception("Must contain 6 values to set time");
  this->set(v[0], v[1], v[2], v[3], v[4], v[5]);
}

void CDate::set(const std::array<int, 6> &v) {
  this->set(v[0], v[1], v[2], v[3], v[4], v[5]);
}

void CDate::set(const std::chrono::system_clock::time_point &t) {
  this->m_datetime = t;
}

void CDate::set(const CDate &v) { this->set(v.time_point()); }

void CDate::fromSeconds(long seconds) {
  this->m_datetime = date::sys_days(c_epoch()) + std::chrono::seconds(seconds);
  return;
}

void CDate::fromMSeconds(long long mseconds) {
  this->fromSeconds(mseconds / 1000);
}

long CDate::toSeconds() const {
  return std::chrono::duration_cast<std::chrono::seconds>(
             this->m_datetime - date::sys_days(c_epoch()))
      .count();
}

long long CDate::toMSeconds() const { return this->toSeconds() * 1000; }

int CDate::year() const {
  s_date d(this->m_datetime);
  return d.year();
}

void CDate::setYear(int year) {
  s_datetime d(this->m_datetime);
  this->set(year, d.month(), d.day(), d.hour(), d.minute(), d.second());
  return;
}

int CDate::month() const {
  s_date d(this->m_datetime);
  return d.month();
}

void CDate::setMonth(int month) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), month, d.day(), d.hour(), d.minute(), d.second());
  return;
}

int CDate::day() const {
  s_date d(this->m_datetime);
  return d.day();
}

void CDate::setDay(int day) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), day, d.hour(), d.minute(), d.second());
  return;
}

int CDate::hour() const {
  s_datetime d(this->m_datetime);
  return d.hour();
}

void CDate::setHour(int hour) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), hour, d.minute(), d.second());
  return;
}

int CDate::minute() const {
  s_datetime d(this->m_datetime);
  return d.minute();
}

void CDate::setMinute(int minute) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), minute, d.second());
  return;
}

int CDate::second() const {
  s_datetime d(this->m_datetime);
  return d.second();
}

void CDate::setSecond(int second) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), d.minute(), second);
  return;
}

void CDate::fromString(const std::string &datestr) {
  int year = stoi(datestr.substr(0, 4));
  int month = stoi(datestr.substr(5, 2));
  int day = stoi(datestr.substr(8, 2));
  int hour = stoi(datestr.substr(11, 2));
  int minute = stoi(datestr.substr(14, 2));
  int second = stoi(datestr.substr(17, 2));
  this->set(year, month, day, hour, minute, second);
}

std::string CDate::toString() const {
  return boost::str(boost::format("%04.4i-%02.2i-%02.2i %02.2i:%02.2i:%02.2i") %
                    this->year() % this->month() % this->day() % this->hour() %
                    this->minute() % this->second());
}

std::chrono::system_clock::time_point CDate::time_point() const {
  return this->m_datetime;
}

CDate CDate::now() { return CDate(std::chrono::system_clock::now()); }
