##------------------------------GPL---------------------------------------##
## This file is part of ADCIRCModules.
##
## (c) 2015-2018 Zachary Cobell
##
## ADCIRCModules is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## ADCIRCModules is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with ADCIRCModules.  If not, see <http://www.gnu.org/licenses/>.
##------------------------------------------------------------------------##

TARGET = _pyadcirc
TEMPLATE = lib
CONFIG -= qt
CONFIG += plugin
CONFIG += no_plugin_name_prefix

pyadcircTarget.target = $$OUT_PWD/adcirc_wrap.cxx
pyadcircTarget.depends = FORCE
pyadcircTarget.commands = C:/swig/swig -I$$PWD/../ADCIRCModules_lib -I$$PWD/../interfaces/qKdtree2 -I$$PWD/../interfaces/qProj4 -I$$PWD/../interfaces/point -c++ -python $$PWD/adcirc.i -o $$OUT_PWD/adcirc_wrap.cxx

win32:INCLUDEPATH+= C:\Python2.7.13\include
unix:INCLUDEPATH += /usr/include/python2.7 /usr/include/tcl8.5

PRE_TARGETDEPS = $$OUT_PWD/adcirc_wrap.cxx
QMAKE_EXTRA_TARGETS += pyadcircTarget

SOURCES += adcirc_wrap.cxx

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/release/ -ladcircmodules
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/debug/ -ladcircmodules
else:unix: LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/ -ladcircmodules

INCLUDEPATH += $$PWD/../ADCIRCModules_lib
DEPENDPATH += $$PWD/../ADCIRCModules_lib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/release/ -lqkdtree2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/debug/ -lqkdtree2
else:unix: LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/ -lqkdtree2

INCLUDEPATH += $$PWD/../interfaces/qKdtree2
DEPENDPATH += $$PWD/../interfaces/qKdtree2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/release/libqkdtree2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/debug/libqkdtree2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/release/qkdtree2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/debug/qkdtree2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/libqkdtree2.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/point/release/ -lpoint
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/point/debug/ -lpoint
else:unix: LIBS += -L$$OUT_PWD/../interfaces/point/ -lpoint

INCLUDEPATH += $$PWD/../interfaces/point
DEPENDPATH += $$PWD/../interfaces/point

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/release/libpoint.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/debug/libpoint.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/release/point.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/debug/point.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/libpoint.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qProj4/release/ -lqproj4
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qProj4/debug/ -lqproj4
else:unix: LIBS += -L$$OUT_PWD/../interfaces/qProj4/ -lqproj4

INCLUDEPATH += $$PWD/../interfaces/qProj4
DEPENDPATH += $$PWD/../interfaces/qProj4

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/release/libqproj4.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/debug/libqproj4.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/release/qproj4.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/debug/qproj4.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/libqproj4.a

win32{
    isEmpty(PREFIX) {
        PREFIX = $$OUT_PWD
    }
    target.path = $$PREFIX/python
    pythonfiles.files = pyadcirc.py
    pythonfiles.path = $$PREFIX/python
    INSTALLS += target pythonfiles
    win32-msvc*{
        lib.files = $$OUT_PWD/release/QADCModules.lib
        lib.path  = $$PREFIX/bin
        INSTALLS  += lib
    }

}
unix{
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    target.path = $$PREFIX/python
    pythonfiles.files = $$PWD/pyadcirc.py
    pythonfiles.path = $$PREFIX/python
    INSTALLS += target pythonfiles

}
