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
#include <time.h>
#include <chrono>
#include <iostream>
#include "boost/format.hpp"

Date::Date() {
  struct tm defaultTime = {0};
  this->m_date = timegm(&defaultTime);
  this->m_epoch_tm = defaultTime;
  this->m_epoch = this->m_date;
}

Date::Date(int year, int month, int day, int hour, int minute, int second) {
  struct tm defaultTime = {0};
  this->m_date = timegm(&defaultTime);
  this->m_epoch_tm = defaultTime;
  this->m_epoch = this->m_date;
  this->set(year, month, day, hour, minute, second);
}

void Date::add(long seconds) {
  this->m_date = this->m_date + seconds;
  this->buildTm();
  this->buildDate();
  return;
}

void Date::buildTm() {
  this->m_tm = *(gmtime(&this->m_date));
  return;
}

void Date::buildDate(int year, int month, int day, int hour, int minute,
                     int second) {
  this->m_tm.tm_sec = second;
  this->m_tm.tm_min = minute;
  this->m_tm.tm_hour = hour;
  this->m_tm.tm_mday = day;
  this->m_tm.tm_mon = month - 1;
  this->m_tm.tm_year = year - 1900;
  this->m_date = timegm(&this->m_tm);
  return;
}

void Date::buildDate() {
  this->m_date = timegm(&this->m_tm);
  return;
}

void Date::set(int year, int month, int day, int hour, int minute, int second) {
  this->buildDate(year, month, day, hour, minute, second);
  this->buildTm();
  return;
}

void Date::get(int year, int month, int day, int hour, int minute, int second) {
  year = this->year();
  month = this->month();
  day = this->day();
  hour = this->hour();
  minute = this->minute();
  second = this->second();
  return;
}

void Date::fromSeconds(long long seconds) {
  this->m_date = this->m_epoch + seconds;
  this->buildTm();
  this->buildDate();
  return;
}

void Date::fromMSeconds(long long mseconds) {
  this->fromSeconds(mseconds / 1000);
}

long long Date::toSeconds() { return this->m_date - this->m_epoch; }

long long Date::toMSeconds() { return this->toSeconds() * 1000; }

int Date::year() { return this->m_tm.tm_year + 1900; }

void Date::setYear(int year) {
  this->m_tm.tm_year = year - 1900;
  this->buildDate();
  this->buildTm();
  return;
}

int Date::month() { return this->m_tm.tm_mon + 1; }

void Date::setMonth(int month) {
  this->m_tm.tm_mon = month - 1;
  this->buildDate();
  this->buildTm();
  return;
}

int Date::day() { return this->m_tm.tm_mday; }

void Date::setDay(int day) {
  this->m_tm.tm_mday = day;
  this->buildDate();
  this->buildTm();
  return;
}

int Date::hour() { return this->m_tm.tm_hour; }

void Date::setHour(int hour) {
  this->m_tm.tm_hour = hour;
  this->buildDate();
  this->buildTm();
  return;
}

int Date::minute() { return this->m_tm.tm_min; }

void Date::setMinute(int minute) {
  this->m_tm.tm_min = minute;
  this->buildDate();
  this->buildTm();
  return;
}

int Date::second() { return this->m_tm.tm_sec; }

void Date::setSecond(int second) {
  this->m_tm.tm_sec = second;
  this->buildDate();
  this->buildTm();
  return;
}

void Date::fromString(const std::string &datestr) {
  int year = stoi(datestr.substr(0, 4));
  int month = stoi(datestr.substr(5, 2));
  int day = stoi(datestr.substr(7, 2));
  int hour = stoi(datestr.substr(9, 2));
  int minute = stoi(datestr.substr(11, 2));
  int second = stoi(datestr.substr(13, 2));
  this->set(year, month, day, hour, minute, second);
}

std::string Date::toString() {
  return boost::str(boost::format("%04.4i-%02.2i-%02.2i %02.2i:%02.2i:%02.2i") %
                    this->year() % this->month() % this->day() % this->hour() %
                    this->minute() % this->second());
}

Date Date::now() {
  auto now = std::chrono::system_clock::now();
  const std::time_t t = std::chrono::system_clock::to_time_t(now);
  std::tm *t2 = gmtime(&t);
  Date d(t2->tm_year + 1900, t2->tm_mon + 1, t2->tm_mday, t2->tm_hour,
         t2->tm_min, t2->tm_sec);
  return d;
}
