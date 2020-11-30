##------------------------------GPL---------------------------------------##
## This file is part of ADCIRCModules.
##
## (c) 2015-2019 Zachary Cobell
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
CONFIG += c++14
#CONFIG += static

DEFINES += ADCIRCMODULES_LIBRARY

#...Abseil path
ABSEILPATH = $$PWD/../thirdparty/abseil

#...Boost Library
BOOSTPATH     = $$PWD/../thirdparty/boost_1_66_0

#...Nanoflann
NANOFLANNPATH  = $$PWD/../thirdparty/nanoflann/include

#...Date
DATEPATH = $$PWD/../thirdparty/date/include/date

#...OpenSSL
macx{
OPENSSLPATH = /usr/local/Cellar/openssl/1.0.2s
}

INCLUDEPATH += $$OPENSSLPATH/include
LIBS += -L$$OPENSSLPATH/lib -lssl -lcrypto

INCLUDEPATH += /usr/include/gdal

win32 {
    #...Set extension for a windows dll (avoids tracking version numbering in filename)
    #TARGET_EXT = .dll

    DEFINES+= USE_GDAL
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

macx{
    isEmpty(NETCDFHOME){
        LIBS += -lnetcdf
        INCLUDEPATH += /usr/local/Cellar/netcdf/4.7.4/include
        INCLUDEPATH += /usr/local/Cellar/hdf5/1.10.5_1/include
    }else{
        LIBS += -L$$NETCDFHOME/lib -lnetcdf
        INCLUDEPATH += $$NETCDFHOME/include
    }
    isEmpty(GDALHOME){
        INCLUDEPATH += /usr/local/Cellar/gdal/2.4.2_2/include
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
    cdate.cpp \
    facetable.cpp \
    formatting.cpp \
    griddata_private.cpp \
    harmonicsoutput_private.cpp \
    harmonicsrecord_private.cpp \
    adchash.cpp \
    adchash_private.cpp \
    kdtree_private.cpp \
    mesh_private.cpp \
    nodalattributes_private.cpp \
    nodetable.cpp \
    outputmetadata.cpp \
    readoutput.cpp \
    stationinterpolation.cpp \
    stationinterpolationoptions.cpp \
    stringconversion.cpp \
    boundary.cpp \
    element.cpp \
    node.cpp \
    attribute.cpp \
    attributemetadata.cpp \
    outputrecord.cpp \
    config.cpp \
    filetypes.cpp \
    meshchecker.cpp \
    elementtable.cpp \
    multithreading.cpp \
    griddata.cpp \
    mesh.cpp \
    harmonicsoutput.cpp \
    harmonicsrecord.cpp \
    nodalattributes.cpp \
    fileio.cpp \
    rasterdata.cpp \
    pixel.cpp \
    constants.cpp \
    kdtree.cpp \
    logging.cpp \
    topology.cpp \
    writeoutput.cpp \
    hmdf.cpp \
    hmdfstation.cpp \
    netcdftimeseries.cpp \
    projection.cpp

HEADERS += \
    adcirc_outputfiles.h \
    adcircmodules_global.h \
    adcmap.h \
    cdate.h \
    face.h \
    facetable.h \
    formatting.h \
    fpcompare.h \
    griddata_private.h \
    harmonicsoutput_private.h \
    harmonicsrecord_private.h \
    adchash.h \
    adchash_private.h \
    hashtype.h \
    kdtree_private.h \
    mesh_private.h \
    nodalattributes_private.h \
    nodetable.h \
    outputfile.h \
    outputmetadata.h \
    readoutput.h \
    stationinterpolation.h \
    stationinterpolationoptions.h \
    stringconversion.h \
    boundary.h \
    element.h \
    node.h \
    attribute.h \
    attributemetadata.h \
    outputrecord.h \
    adcirc_codes.h \
    config.h \
    filetypes.h \
    meshchecker.h \
    elementtable.h \
    multithreading.h \
    griddata.h \
    interpolationmethods.h \
    mesh.h \
    harmonicsoutput.h \
    harmonicsrecord.h \
    nodalattributes.h \
    adcircmodules.h \
    fileio.h \
    rasterdata.h \
    pixel.h \
    constants.h \
    kdtree.h \
    logging.h \
    default_values.h \
    topology.h \
    writeoutput.h \
    hmdf.h \
    hmdfstation.h \
    netcdftimeseries.h \
    projection.h

INCLUDEPATH += $$BOOSTPATH $$ABSEILPATH $$NANOFLANNPATH $$DATEPATH
INCLUDEPATH += $$PWD/../thirdparty/shapelib
INCLUDEPATH += $$PWD/../thirdparty/proj-7.2.0/src

unix {
    INCLUDEPATH += /usr/include/hdf5/serial
}

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
