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
#CONFIG += static

DEFINES += ADCIRCMODULES_LIBRARY

#...Abseil path
ABSEILPATH = $$PWD/../thirdparty/abseil

#...Boost Library
BOOSTPATH     = $$PWD/../thirdparty/boost_1_66_0

win32 {
    #...Set extension for a windows dll (avoids tracking version numbering in filename)
    #TARGET_EXT = .dll

    INCLUDEPATH += C:/OSGeo4W64/include
    LIBS+= -LC:/OSGeo4W64/lib -lgdal_i

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
    LIBS+= -lgdal
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
    boundary.cpp \
    element.cpp \
    node.cpp \
    attribute.cpp \
    attributemetadata.cpp \
    outputfile.cpp \
    outputrecord.cpp \
    config.cpp \
    error.cpp \
    filetypes.cpp \
    meshchecker.cpp \
    elementtable.cpp \
    multithreading.cpp \
    griddata.cpp \
    mesh.cpp \
    harmonicsoutput.cpp \
    harmonicsrecord.cpp \
    pimpl/griddata_impl.cpp \
    pimpl/mesh_impl.cpp \
    pimpl/harmonicsoutput_impl.cpp \
    pimpl/harmonicsrecord_impl.cpp \
    pimpl/nodalattributes_impl.cpp \
    nodalattributes.cpp \
    fileio.cpp

HEADERS += \
    adcircmodules_global.h \
    split.h \
    stringconversion.h \
    boundary.h \
    element.h \
    node.h \
    attribute.h \
    attributemetadata.h \
    outputfile.h \
    outputrecord.h \
    adcirc_codes.h \
    config.h \
    error.h \
    filetypes.h \
    meshchecker.h \
    elementtable.h \
    multithreading.h \
    griddata.h \
    interpolationmethods.h \
    mesh.h \
    harmonicsoutput.h \
    harmonicsrecord.h \
    pimpl/harmonicsoutput_impl.h \
    pimpl/mesh_impl.h \
    pimpl/griddata_impl.h \
    pimpl/harmonicsrecord_impl.h \
    pimpl/nodalattributes_impl.h \
    nodalattributes.h \
    adcircmodules.h \
    fileio.h

INCLUDEPATH += $$BOOSTPATH $$ABSEILPATH
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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/EzProj/src/release/ -lezproj
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../thirdparty/EzProj/src/debug/ -lezproj
else:unix: LIBS += -L$$OUT_PWD/../thirdparty/EzProj/src/ -lezproj

INCLUDEPATH += $$PWD/../thirdparty/EzProj/src
DEPENDPATH += $$PWD/../thirdparty/EzProj/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/EzProj/src/release/libezproj.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/EzProj/src/debug/libezproj.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/EzProj/src/release/ezproj.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/EzProj/src/debug/ezproj.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../thirdparty/EzProj/src/libezproj.a

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

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libraries/kdtree2/release/ -lkdtree2lib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libraries/kdtree2/debug/ -lkdtree2lib
else:unix: LIBS += -L$$OUT_PWD/../libraries/kdtree2/ -lkdtree2lib

INCLUDEPATH += $$PWD/../libraries/kdtree2
DEPENDPATH += $$PWD/../libraries/kdtree2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/kdtree2/release/libkdtree2lib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/kdtree2/debug/libkdtree2lib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/kdtree2/release/kdtree2lib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/kdtree2/debug/kdtree2lib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libraries/kdtree2/libkdtree2lib.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libraries/constants/release/ -lconstants
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libraries/constants/debug/ -lconstants
else:unix: LIBS += -L$$OUT_PWD/../libraries/constants/ -lconstants

INCLUDEPATH += $$PWD/../libraries/constants
DEPENDPATH += $$PWD/../libraries/constants

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/constants/release/libconstants.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/constants/debug/libconstants.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/constants/release/constants.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/constants/debug/constants.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libraries/constants/libconstants.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libraries/rasterdata/release/ -lrasterdata
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libraries/rasterdata/debug/ -lrasterdata
else:unix: LIBS += -L$$OUT_PWD/../libraries/rasterdata/ -lrasterdata

INCLUDEPATH += $$PWD/../libraries/rasterdata
DEPENDPATH += $$PWD/../libraries/rasterdata

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/rasterdata/release/librasterdata.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/rasterdata/debug/librasterdata.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/rasterdata/release/rasterdata.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../libraries/rasterdata/debug/rasterdata.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../libraries/rasterdata/librasterdata.a
