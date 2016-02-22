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
    adcirc_node.cpp \
    adcirc_element.cpp \
    adcirc_boundary.cpp \
    adcirc_mesh.cpp

HEADERS +=\
        QADCModules_global.h \
        QADCModules.h \
    QADCModules_flags.h \
    adcirc_node.h \
    adcirc_element.h \
    adcirc_boundary.h \
    adcirc_mesh.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
