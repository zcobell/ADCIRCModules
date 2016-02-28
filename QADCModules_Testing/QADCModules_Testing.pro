#-------------------------------------------------
#
# Project created by QtCreator 2016-02-22T09:12:44
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = QADCModules_Testing
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../.

debug{
LIBS += -L$$PWD/../../build-QADCModules-Desktop_Qt_5_5_0_MSVC2013_64bit-Debug/debug -lQADCModules
}

release{
LIBS += -L$$PWD/../../build-QADCModules-Desktop_Qt_5_5_0_MSVC2013_64bit-Release/release -lQADCModules
}

SOURCES += main.cpp
