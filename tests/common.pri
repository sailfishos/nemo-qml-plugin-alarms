SRCDIR = ../../src/
include(../src/src.pro)
INCLUDEPATH += $$SRCDIR
DEPENDPATH = $$INCLUDEPATH
QT += testlib
TEMPLATE = app
CONFIG -= app_bundle

DEFINES += USE_VOLAND_TEST_INTERFACE

target.path = /opt/tests/nemo-qml-plugins-qt5/alarms
INSTALLS += target
