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

QT       -= gui

TARGET = adcircmodules
TEMPLATE = lib
VERSION = 0.1.0
CONFIG -= qt
CONFIG += c++11

DEFINES += ADCIRCMODULES_LIBRARY

#...Boost Library
BOOSTPATH     = $$PWD/../../../thirdparty/boost_1_66_0

win32 {
    #...Set extension for a windows dll (avoids tracking version numbering in filename)
    TARGET_EXT = .dll

    #...Include the netCDF header
    INCLUDEPATH += $$PWD/../../../thirdparty/netcdf/include

    #...Check for Visual C++ Compilers
    *msvc*{
        contains(QT_ARCH, i386){
            message("MSVC-32 Compiler in use")
            LIBS += -L$$PWD/../../../thirdparty/netcdf/libs_vc32 -lnetcdf -lhdf5 -lzlib -llibcurl_imp
        }else{
            message("MSVC-64 Compiler in use")
            LIBS += -L$$PWD/../../../thirdparty/netcdf/libs_vc64 -lnetcdf -lhdf5 -lzlib -llibcurl_imp
        }
    }

    #...Check for MinGW-32
    win32-g++ {
        LIBS += -L$$PWD/../../../thirdparty/netcdf/bin_32 -lnetcdf -lhdf5 -lz -lcurl
    }
}

#...For unix, we assume netCDF exists in the path if it is not provided on the command line
#   via qmake NETCDFHOME=$(NETCDFPATH)
unix:!macx{
    isEmpty(NETCDFHOME){
        LIBS += -lnetcdf
    }else{
        LIBS += -L$$NETCDFHOME/lib -lnetcdf
        INCLUDEPATH += $$NETCDFHOME/include
    }
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
    ../../adcirc/io/split.cpp \
    ../../adcirc/io/stringconversion.cpp \
    ../../adcirc/io/io.cpp \
    ../../adcirc/geometry/boundary.cpp \
    ../../adcirc/geometry/mesh.cpp \
    ../../adcirc/geometry/element.cpp \
    ../../adcirc/geometry/node.cpp \
    ../../adcirc/modelparameters/attribute.cpp \
    ../../adcirc/modelparameters/attributemetadata.cpp \
    ../../adcirc/modelparameters/nodalattributes.cpp \
    ../../adcirc/output/outputfile.cpp \
    ../../adcirc/output/outputrecord.cpp \
    ../../adcirc/config.cpp \
    ../output/harmonicsoutput.cpp \
    ../output/harmonicsrecord.cpp \
    ../architecture/error.cpp

HEADERS += \
    ../../adcirc/adcircmodules_global.h \
    ../../adcirc/io/split.h \
    ../../adcirc/io/stringconversion.h \
    ../../adcirc/io/io.h \
    ../../adcirc/io/parsers.h \
    ../../adcirc/geometry/boundary.h \
    ../../adcirc/geometry/element.h \
    ../../adcirc.h \
    ../../adcirc/geometry/mesh.h \
    ../../adcirc/geometry/node.h \
    ../../adcirc/modelparameters/attribute.h \
    ../../adcirc/modelparameters/attributemetadata.h \
    ../../adcirc/modelparameters/nodalattributes.h \
    ../../adcirc/output/outputfile.h \
    ../../adcirc/output/outputrecord.h \
    ../../adcirc/adcirc_codes.h \
    ../../adcirc/config.h \
    ../output/harmonicsoutput.h \
    ../output/harmonicsrecord.h \
    ../architecture/error.h

INCLUDEPATH += $$BOOSTPATH
INCLUDEPATH += ../..
INCLUDEPATH += ../../../thirdparty/shapelib

win32{
    isEmpty(PREFIX) {
        PREFIX = $$OUT_PWD
    }
    target.path = $$PREFIX/bin
    INSTALLS += target
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
    target.path = $$PREFIX/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../point/release/ -lpoint
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../point/debug/ -lpoint
else:unix: LIBS += -L$$OUT_PWD/../point/ -lpoint

INCLUDEPATH += $$PWD/../point
DEPENDPATH += $$PWD/../point

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/release/libpoint.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/debug/libpoint.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/release/point.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../point/debug/point.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../point/libpoint.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qKdtree2/release/ -lqkdtree2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qKdtree2/debug/ -lqkdtree2
else:unix: LIBS += -L$$OUT_PWD/../qKdtree2/ -lqkdtree2

INCLUDEPATH += $$PWD/../qKdtree2
DEPENDPATH += $$PWD/../qKdtree2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qKdtree2/release/libqkdtree2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qKdtree2/debug/libqkdtree2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qKdtree2/release/qkdtree2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qKdtree2/debug/qkdtree2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../qKdtree2/libqkdtree2.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qProj4/release/ -lqproj4
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qProj4/debug/ -lqproj4
else:unix: LIBS += -L$$OUT_PWD/../qProj4/ -lqproj4

INCLUDEPATH += $$PWD/../qProj4
DEPENDPATH += $$PWD/../qProj4

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qProj4/release/libqproj4.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qProj4/debug/libqproj4.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qProj4/release/qproj4.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qProj4/debug/qproj4.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../qProj4/libqproj4.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../shapelib/release/ -lshapelib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../shapelib/debug/ -lshapelib
else:unix: LIBS += -L$$OUT_PWD/../shapelib/ -lshapelib

INCLUDEPATH += $$PWD/../shapelib
DEPENDPATH += $$PWD/../shapelib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shapelib/release/libshapelib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shapelib/debug/libshapelib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shapelib/release/shapelib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../shapelib/debug/shapelib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../shapelib/libshapelib.a

#...Ensure that git is in the system path. If not using GIT comment these two lines
GIT_VERSION = $$system(git --git-dir $$PWD/../../../.git --work-tree $$PWD/.. describe --always --tags)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"
