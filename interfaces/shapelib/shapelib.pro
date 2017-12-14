
TARGET = shapelib
TEMPLATE = lib
CONFIG += static
CONFIG -= qt

#...Shapelib Library
SHAPELIBPATH  = $$PWD/../../thirdparty/shapelib

#...Shapelib Sources
SOURCES_SHAPELIB = $$SHAPELIBPATH/shpopen.c $$SHAPELIBPATH/dbfopen.c $$SHAPELIBPATH/safileio.c $$SHAPELIBPATH/shptree.c

SOURCES += $$SOURCES_SHAPELIB

INCLUDEPATH += $$SHAPELIBPATH
