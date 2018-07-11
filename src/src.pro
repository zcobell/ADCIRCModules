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
BOOSTPATH     = $$PWD/../thirdparty/boost_1_66_0

win32 {
    #...Set extension for a windows dll (avoids tracking version numbering in filename)
    TARGET_EXT = .dll

    #...Include the netCDF header
    INCLUDEPATH += $$PWD/../thirdparty/netcdf/include

    #...Check for Visual C++ Compilers
    *msvc*{
        contains(QT_ARCH, i386){
            message("MSVC-32 Compiler in use")
            LIBS += -L$$PWD/../thirdparty/netcdf/libs_vc32 -lnetcdf -lhdf5 -lzlib -llibcurl_imp
        }else{
            message("MSVC-64 Compiler in use")
            LIBS += -L$$PWD/../thirdparty/netcdf/libs_vc64 -lnetcdf -lhdf5 -lzlib -llibcurl_imp
        }
    }

    #...Check for MinGW-32
    win32-g++ {
        LIBS += -L$$PWD/../thirdparty/netcdf/bin_32 -lnetcdf -lhdf5 -lz -lcurl
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
    split.cpp \
    stringconversion.cpp \
    io.cpp \
    boundary.cpp \
    mesh.cpp \
    element.cpp \
    node.cpp \
    attribute.cpp \
    attributemetadata.cpp \
    nodalattributes.cpp \
    outputfile.cpp \
    outputrecord.cpp \
    config.cpp \
    harmonicsoutput.cpp \
    harmonicsrecord.cpp \
    error.cpp \
    filetypes.cpp

HEADERS += \
    adcircmodules_global.h \
    split.h \
    stringconversion.h \
    io.h \
    parsers.h \
    boundary.h \
    element.h \
    adcirc.h \
    mesh.h \
    node.h \
    attribute.h \
    attributemetadata.h \
    nodalattributes.h \
    outputfile.h \
    outputrecord.h \
    adcirc_codes.h \
    config.h \
    harmonicsoutput.h \
    harmonicsrecord.h \
    error.h \
    filetypes.h

INCLUDEPATH += $$BOOSTPATH
INCLUDEPATH += $$PWD/../thirdparty/shapelib

win32{
    isEmpty(PREFIX) {
        PREFIX = $$OUT_PWD
    }
    target.path = $$PREFIX/bin
    INSTALLS += target
    win32-msvc*{
        lib.files = $$OUT_PWD/release/ADCIRCModules.lib
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

#...Ensure that git is in the system path. If not using GIT comment these two lines
GIT_VERSION = $$system(git --git-dir $$PWD/../.git --work-tree $$PWD/.. describe --always --tags)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libraries/point/release/ -lpoint
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libraries/point/debug/ -lpoint
else:unix: LIBS += -L$$OUT_PWD/../libraries/point/ -lpoint

INCLUDEPATH += $$PWD/../libraries/point
DEPENDPATH += $$PWD/../libraries/point

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/point/release/libpoint.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/point/debug/libpoint.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/point/release/point.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/point/debug/point.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libraries/point/libpoint.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libraries/qKdtree2/release/ -lqkdtree2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libraries/qKdtree2/debug/ -lqkdtree2
else:unix: LIBS += -L$$OUT_PWD/../libraries/qKdtree2/ -lqkdtree2

INCLUDEPATH += $$PWD/../libraries/qKdtree2
DEPENDPATH += $$PWD/../libraries/qKdtree2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qKdtree2/release/libqkdtree2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qKdtree2/debug/libqkdtree2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qKdtree2/release/qkdtree2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qKdtree2/debug/qkdtree2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libraries/qKdtree2/libqkdtree2.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libraries/qProj4/release/ -lqproj4
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libraries/qProj4/debug/ -lqproj4
else:unix: LIBS += -L$$OUT_PWD/../libraries/qProj4/ -lqproj4

INCLUDEPATH += $$PWD/../libraries/qProj4
DEPENDPATH += $$PWD/../libraries/qProj4

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qProj4/release/libqproj4.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qProj4/debug/libqproj4.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qProj4/release/qproj4.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/qProj4/debug/qproj4.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libraries/qProj4/libqproj4.a


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libraries/shapelib/release/ -lshapelib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libraries/shapelib/debug/ -lshapelib
else:unix: LIBS += -L$$OUT_PWD/../libraries/shapelib/ -lshapelib

INCLUDEPATH += $$PWD/../libraries/shapelib
DEPENDPATH += $$PWD/../libraries/shapelib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/shapelib/release/libshapelib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/shapelib/debug/libshapelib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/shapelib/release/shapelib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/shapelib/debug/shapelib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libraries/shapelib/libshapelib.a
