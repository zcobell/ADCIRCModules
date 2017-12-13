
TARGET = qKdtree2
TEMPLATE = lib
CONFIG += static

#...Boost Library
BOOSTPATH     = $$PWD/../../thirdparty/boost-library

#...KDTREE
KDTREEPATH    = $$PWD/../../thirdparty/kdtree

#...KDTREE2 Sources
SOURCES_KDTREE2 = $$KDTREEPATH/kdtree2.cpp

HEADERS += qkdtree2.h

SOURCES += qkdtree2.cpp $$SOURCES_KDTREE2

INCLUDEPATH += $$KDTREEPATH $$BOOSTPATH
