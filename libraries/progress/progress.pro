
QT       -= core gui

TARGET = progress
TEMPLATE = lib
CONFIG += staticlib

SOURCES += $$PWD/../../thirdparty/progressbar/lib/progressbar.c \
           $$PWD/../../thirdparty/progressbar/lib/statusbar.c

HEADERS += $$PWD/../../thirdparty/progressbar/include/progressbar/progressbar.h \
           $$PWD/../../thirdparty/progressbar/include/progressbar/statusbar.h

INCLUDEPATH += $$PWD/../../thirdparty/progressbar/include/progressbar

unix {
    target.path = /usr/lib
    INSTALLS += target
}
