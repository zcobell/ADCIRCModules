#-------------------------------GPL-------------------------------------#
#
# QADCModules - A library for working with ADCIRC models
# Copyright (C) 2016  Zach Cobell
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------#

QT       += core

QT       -= gui

TARGET = QADCModules_Testing
CONFIG   += console
CONFIG   -= app_bundle
CONFIG   += testcase

TEMPLATE = app

INCLUDEPATH += ../QADCModules_lib/include

CONFIG(debug, debug | release): LIBS += -L$$OUT_PWD/../QADCModules_lib/debug -lQADCModules
CONFIG(release, debug | release): LIBS += -L$$OUT_PWD/../QADCModules_lib/release -lQADCModules

SOURCES += main.cpp
