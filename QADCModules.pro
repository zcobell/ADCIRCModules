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

DEFINES += QADCMODULES_LIBRARY

SOURCES += QADCModules.cpp \
    adcirc_node.cpp \
    adcirc_element.cpp \
    adcirc_boundary.cpp \
    adcirc_mesh.cpp \
    adcirc_errors.cpp \
    adcirc_nodalparameter.cpp \
    adcirc_nodalattributes.cpp

HEADERS +=\
        QADCModules_global.h \
        QADCModules.h \
    adcirc_node.h \
    adcirc_element.h \
    adcirc_boundary.h \
    adcirc_mesh.h \
    adcirc_errors.h \
    adcirc_nodalparameter.h \
    adcirc_nodalattributes.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
