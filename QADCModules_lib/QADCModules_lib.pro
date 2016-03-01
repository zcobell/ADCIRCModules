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

QT       -= gui

TARGET = QADCModules 
TEMPLATE = lib
VERSION = 0.1.0.0

win32{
TARGET_EXT = .dll
}

INCLUDEPATH += include

DEFINES += QADCMODULES_LIBRARY

SOURCES += \
    src/QADCModules.cpp \
    src/adcirc_node.cpp \
    src/adcirc_element.cpp \
    src/adcirc_boundary.cpp \
    src/adcirc_mesh.cpp \
    src/adcirc_errors.cpp \
    src/adcirc_nodalparameter.cpp \
    src/adcirc_nodalattributes.cpp

HEADERS +=\
    include/QADCModules_global.h \
    include/QADCModules.h \
    include/adcirc_node.h \
    include/adcirc_element.h \
    include/adcirc_boundary.h \
    include/adcirc_mesh.h \
    include/adcirc_errors.h \
    include/adcirc_nodalparameter.h \
    include/adcirc_nodalattributes.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
