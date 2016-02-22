#-------------------------------------------------
#
# Project created by QtCreator 2016-02-21T02:23:59
#
#-------------------------------------------------

QT       -= gui

TARGET = QADCModules 
TEMPLATE = lib

DEFINES += QADCMODULES_LIBRARY

SOURCES += QADCModules.cpp \
    QADCmesh.cpp

HEADERS +=\
        QADCModules_global.h \
        QADCModules.h \
    QADCModules_flags.h \
    QADCmesh.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
