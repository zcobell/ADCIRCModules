#-------------------------------------------------
#
# Project created by QtCreator 2017-12-14T12:35:57
#
#-------------------------------------------------

TARGET = _pyadcirc
TEMPLATE = lib
CONFIG -= qt
CONFIG += plugin
CONFIG += no_plugin_name_prefix

pyadcircTarget.target = pyadcirc_wrap.cxx
pyadcircTarget.depends = FORCE
pyadcircTarget.commands = swig -I$$PWD/../ADCIRCModules_lib -c++ -python $$PWD/pyadcirc.i

INCLUDEPATH += /usr/include/python2.7 /usr/include/tcl8.5

PRE_TARGETDEPS = pyadcirc_wrap.cxx
QMAKE_EXTRA_TARGETS += pyadcircTarget

SOURCES += pyadcirc_wrap.cxx

unix {
    target.path = /usr/lib
    INSTALLS += target
}

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/release/ -ladcircmodules
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/debug/ -ladcircmodules
else:unix: LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/ -ladcircmodules

INCLUDEPATH += $$PWD/../ADCIRCModules_lib
DEPENDPATH += $$PWD/../ADCIRCModules_lib

pythonImport.files = $$PWD/pyadcirc.py

INSTALLS += pythonImport

win32{
    isEmpty(PREFIX) {
        PREFIX = $$OUT_PWD
    }
    target.path = $$PREFIX/python
    pythonfiles.files = pyadcirc.py
    pythonfiles.path = $$PREFIX/python
    INSTALLS += target pythonfiles
    win32-msvc*{
        lib.files = $$OUT_PWD/release/QADCModules.lib
        lib.path  = $$PREFIX/bin
        INSTALLS  += lib
    }

}
unix{
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    target.path = $$PREFIX/python
    pythonfiles.files = pyadcirc.py
    pythonfiles.path = $$PREFIX/python
    INSTALLS += target pythonfiles

}
