QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        highwatermarkoptions.cpp \
        hwmstats.cpp \
        location.cpp \
        locations.cpp \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#...Boost Library
BOOSTPATH     = $$PWD/../../thirdparty/boost_1_66_0

#...CXXOPTS Path
CXXOPTSPATH   = $$PWD/../../thirdparty/cxxopts

#...Shapelib Library
SHAPELIBPATH  = $$PWD/../../thirdparty/shapelib

INCLUDEPATH += $$BOOSTPATH $$CXXOPTSPATH $$SHAPELIBPATH

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../src/release/ -ladcircmodules
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../src/debug/ -ladcircmodules
else:unix: LIBS += -L$$OUT_PWD/../../src/ -ladcircmodules

INCLUDEPATH += $$PWD/../../src
DEPENDPATH += $$PWD/../../src

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../thirdparty/ezproj/src/release/ -lezproj
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../thirdparty/ezproj/src/debug/ -lezproj
else:unix: LIBS += -L$$OUT_PWD/../../thirdparty/ezproj/src/ -lezproj

INCLUDEPATH += $$PWD/../../thirdparty/ezproj/src
DEPENDPATH += $$PWD/../../thirdparty/ezproj/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../thirdparty/ezproj/src/release/libezproj.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../thirdparty/ezproj/src/debug/libezproj.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../thirdparty/ezproj/src/release/ezproj.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../thirdparty/ezproj/src/debug/ezproj.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../thirdparty/ezproj/src/libezproj.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../libraries/shapelib/release/ -lshapelib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../libraries/shapelib/debug/ -lshapelib
else:unix: LIBS += -L$$OUT_PWD/../../libraries/shapelib/ -lshapelib

INCLUDEPATH += $$PWD/../../libraries/shapelib
DEPENDPATH += $$PWD/../../libraries/shapelib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libraries/shapelib/release/libshapelib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libraries/shapelib/debug/libshapelib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libraries/shapelib/release/shapelib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../libraries/shapelib/debug/shapelib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../libraries/shapelib/libshapelib.a

HEADERS += \
    highwatermarkoptions.h \
    hwmstats.h \
    location.h \
    locations.h
