TARGET = nemoalarms
PLUGIN_IMPORT_PATH = org/nemomobile/alarms

TEMPLATE = lib
CONFIG += qt plugin hide_symbols
QT += declarative

target.path = $$[QT_INSTALL_IMPORTS]/$$PLUGIN_IMPORT_PATH
INSTALLS += target

qmldir.files += $$_PRO_FILE_PWD_/qmldir
qmldir.path +=  $$[QT_INSTALL_IMPORTS]/$$$$PLUGIN_IMPORT_PATH
INSTALLS += qmldir

QT += dbus

CONFIG += link_pkgconfig
PKGCONFIG += timed timed-voland

SOURCES += plugin.cpp \
    alarmsbackendmodel.cpp \
    alarmsbackendmodel_p.cpp \
    alarmobject.cpp \
    alarmhandlerinterface.cpp \
    alarmdialogobject.cpp

HEADERS += alarmsbackendmodel.h \
    alarmsbackendmodel_p.h \
    alarmobject.h \
    alarmhandlerinterface.h \
    alarmdialogobject.h
