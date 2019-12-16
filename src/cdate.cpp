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
  std::chrono::system_clock::time_point t;
  date::year_month_day dd;
  date::hh_mm_ss<std::chrono::duration<long int, std::ratio<1, 1000000000>>> tt;

 public:
  s_datetime(const std::chrono::system_clock::time_point &t)
      : t(t),
        dd(date::year_month_day(date::floor<date::days>(t))),
        tt(date::make_time(t - date::sys_days(dd))) {}
  date::year_month_day ymd() { return dd; }
  int year() const { return int(dd.year()); }
  unsigned month() const { return unsigned(dd.month()); }
  unsigned day() const { return unsigned(dd.day()); }
  int hour() const { return tt.hours().count(); }
  int minute() const { return tt.minutes().count(); }
  int second() const { return tt.seconds().count(); }
  int milliseconds() const {
    CDate c(year(), month(), day(), hour(), minute(), second());
    return std::chrono::duration_cast<std::chrono::milliseconds>(t -
                                                                 c.time_point())
        .count();
  }
};

date::year_month_day normalize(date::year_month_day ymd) {
  ymd += date::months{0};
  ymd = date::sys_days{ymd};
  return ymd;
}

constexpr date::year_month_day c_epoch() {
  return date::year_month_day(date::year(1970) / 1 / 1);
}

CDate::CDate() { this->set(1970, 1, 1, 0, 0, 0, 0); }

CDate::CDate(const std::chrono::system_clock::time_point &t) { this->set(t); }

CDate::CDate(const std::vector<int> &v) { this->set(v); }

CDate::CDate(const CDate &d) { this->set(d.get()); }

CDate::CDate(int year, int month, int day, int hour, int minute, int second,
             int millisecond) {
  this->set(year, month, day, hour, minute, second, millisecond);
}

void CDate::addSeconds(const long &value) { this->m_datetime += seconds(value); }

void CDate::addMinutes(const long &value) { this->m_datetime += minutes(value); }

void CDate::addHours(const long &value) { this->m_datetime += hours(value); }

void CDate::addDays(const long &value) { this->m_datetime += days(value); }

void CDate::addWeeks(const long &value) { this->m_datetime += weeks(value); }

void CDate::addMonths(const long &value) { this->m_datetime += months(value); }

void CDate::addYears(const long &value) { this->m_datetime += years(value); }

bool CDate::operator<(const CDate &d) const {
  return this->time_point() < d.time_point();
}

bool CDate::operator==(const CDate &d) const {
  return this->time_point() == d.time_point();
}

bool CDate::operator!=(const CDate &d) const {
  return this->time_point() != d.time_point();
}

CDate &CDate::operator-=(const CDate::months &rhs) {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd -= date::months(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

CDate &CDate::operator-=(const CDate::years &rhs) {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd -= date::years(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

CDate &CDate::operator+=(const CDate::months &rhs) {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd += date::months(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

CDate &CDate::operator+=(const CDate::years &rhs) {
  s_datetime d(this->m_datetime);
  date::year_month_day dd = d.ymd();
  dd += date::years(rhs);
  dd = normalize(dd);
  this->set(int(dd.year()), unsigned(dd.month()), unsigned(dd.day()),
            this->hour(), this->minute(), this->second(), this->millisecond());
  return *this;
}

void CDate::set(int year, int month, int day, int hour, int minute, int second,
                int millisecond) {
  auto ymd = date::year(year) / date::month(month) / date::day(day);
  if (!ymd.ok()) {
    adcircmodules_throw_exception("Invalid date");
  }
  this->m_datetime = date::sys_days(ymd) + std::chrono::hours(hour) +
                     std::chrono::minutes(minute) +
                     std::chrono::seconds(second) +
                     std::chrono::milliseconds(millisecond);
  return;
}

std::vector<int> CDate::get() const {
  s_datetime time(this->m_datetime);
  std::vector<int> v(7);
  v[0] = time.year();
  v[1] = time.month();
  v[2] = time.day();
  v[3] = time.hour();
  v[4] = time.minute();
  v[5] = time.second();
  v[6] = time.milliseconds();
  return v;
}

void CDate::set(const std::vector<int> &v) {
  std::vector<int> v2(7);
  std::copy(v.begin(), v.end(), v2.begin());

  this->set(v[0], v[1], v[2], v[3], v[4], v[5], v[6]);
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

int CDate::millisecond() const {
  s_datetime d(this->m_datetime);
  return d.milliseconds();
}

void CDate::setMillisecond(int milliseconds) {
  s_datetime d(this->m_datetime);
  this->set(d.year(), d.month(), d.day(), d.hour(), d.minute(), d.second(),
            milliseconds);
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
  return boost::str(
      boost::format("%04.4i-%02.2i-%02.2i %02.2i:%02.2i:%02.2i.%04.4i") %
      this->year() % this->month() % this->day() % this->hour() %
      this->minute() % this->second() % this->millisecond());
}

std::chrono::system_clock::time_point CDate::time_point() const {
  return this->m_datetime;
}

CDate CDate::now() { return CDate(std::chrono::system_clock::now()); }
