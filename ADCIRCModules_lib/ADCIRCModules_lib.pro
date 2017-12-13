#-------------------------------------------------
#
# Project created by QtCreator 2017-12-09T14:54:28
#
#-------------------------------------------------

QT       -= gui

TARGET = adcircmodules
TEMPLATE = lib
VERSION = 0.1.0

DEFINES += ADCIRCMODULES_LIBRARY

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
    adcircmesh.cpp \
    adcircnode.cpp \
    adcircelement.cpp \
    adcircboundary.cpp

HEADERS += \
    adcircmodules_global.h \ 
    adcircmesh.h \
    adcircnode.h \
    adcircelement.h \
    adcircboundary.h \
    adcircenum.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/release/ -lqKdtree2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/debug/ -lqKdtree2
else:unix: LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/ -lqKdtree2

INCLUDEPATH += $$PWD/../interfaces/qKdtree2
DEPENDPATH += $$PWD/../interfaces/qKdtree2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/release/libqKdtree2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/debug/libqKdtree2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/release/qKdtree2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/debug/qKdtree2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/libqKdtree2.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qProj4/release/ -lqProj4
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qProj4/debug/ -lqProj4
else:unix: LIBS += -L$$OUT_PWD/../interfaces/qProj4/ -lqProj4

INCLUDEPATH += $$PWD/../interfaces/qProj4
DEPENDPATH += $$PWD/../interfaces/qProj4

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/release/libqProj4.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/debug/libqProj4.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/release/qProj4.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/debug/qProj4.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qProj4/libqProj4.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/shapelib/release/ -lshapelib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/shapelib/debug/ -lshapelib
else:unix: LIBS += -L$$OUT_PWD/../interfaces/shapelib/ -lshapelib

INCLUDEPATH += $$PWD/../interfaces/shapelib
DEPENDPATH += $$PWD/../interfaces/shapelib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/shapelib/release/libshapelib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/shapelib/debug/libshapelib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/shapelib/release/shapelib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/shapelib/debug/shapelib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/shapelib/libshapelib.a
