
TARGET = qkdtree2
TEMPLATE = lib
CONFIG += static
CONFIG -= qt

#...Boost Library
BOOSTPATH     = $$PWD/../../thirdparty/boost_1_66_0

#...KDTREE
KDTREEPATH    = $$PWD/../../thirdparty/kdtree

#...KDTREE2 Sources
SOURCES_KDTREE2 = $$KDTREEPATH/kdtree2.cpp

HEADERS += qkdtree2.h

SOURCES += qkdtree2.cpp $$SOURCES_KDTREE2

INCLUDEPATH += $$KDTREEPATH $$BOOSTPATH

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
