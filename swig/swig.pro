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
pyadcircTarget.commands = swig -I$$PWD/../ADCIRCModules_lib -I$$PWD/../interfaces/qKdtree2 -I$$PWD/../interfaces/point -c++ -python $$PWD/adcirc.i

INCLUDEPATH += /usr/include/python2.7 /usr/include/tcl8.5

PRE_TARGETDEPS = pyadcirc_wrap.cxx
QMAKE_EXTRA_TARGETS += pyadcircTarget

SOURCES += pyadcirc_wrap.cxx

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/release/ -ladcircmodules
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/debug/ -ladcircmodules
else:unix: LIBS += -L$$OUT_PWD/../ADCIRCModules_lib/ -ladcircmodules

INCLUDEPATH += $$PWD/../ADCIRCModules_lib
DEPENDPATH += $$PWD/../ADCIRCModules_lib

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/release/ -lqkdtree2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/debug/ -lqkdtree2
else:unix: LIBS += -L$$OUT_PWD/../interfaces/qKdtree2/ -lqkdtree2

INCLUDEPATH += $$PWD/../interfaces/qKdtree2
DEPENDPATH += $$PWD/../interfaces/qKdtree2

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/release/libqkdtree2.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/debug/libqkdtree2.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/release/qkdtree2.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/debug/qkdtree2.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/qKdtree2/libqkdtree2.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../interfaces/point/release/ -lpoint
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../interfaces/point/debug/ -lpoint
else:unix: LIBS += -L$$OUT_PWD/../interfaces/point/ -lpoint

INCLUDEPATH += $$PWD/../interfaces/point
DEPENDPATH += $$PWD/../interfaces/point

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/release/libpoint.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/debug/libpoint.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/release/point.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/debug/point.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../interfaces/point/libpoint.a


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
    pythonfiles.files = $$PWD/pyadcirc.py
    pythonfiles.path = $$PREFIX/python
    INSTALLS += target pythonfiles

}
