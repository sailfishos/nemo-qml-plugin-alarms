SRCDIR = ../../src/
include(../src/src.pro)
INCLUDEPATH += $$SRCDIR
DEPENDPATH = $$INCLUDEPATH
QT += testlib
TEMPLATE = app
CONFIG -= app_bundle

DEFINES += USE_VOLAND_TEST_INTERFACE

equals(QT_MAJOR_VERSION, 4): target.path = /opt/tests/nemo-qml-plugins/alarms
equals(QT_MAJOR_VERSION, 5): target.path = /opt/tests/nemo-qml-plugins-qt5/alarms
INSTALLS += target
