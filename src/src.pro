TARGET = nemoalarms
PLUGIN_IMPORT_PATH = Nemo/Alarms

TEMPLATE = lib
CONFIG += qt plugin hide_symbols
QT -= gui
QT += qml dbus

target.path = $$[QT_INSTALL_QML]/$$PLUGIN_IMPORT_PATH
INSTALLS += target

qmldir.files += qmldir plugins.qmltypes
qmldir.path +=  $$target.path
INSTALLS += qmldir

qmltypes.commands = qmlplugindump -nonrelocatable Nemo.Alarms 1.0 > $$PWD/plugins.qmltypes
QMAKE_EXTRA_TARGETS += qmltypes

CONFIG += link_pkgconfig
PKGCONFIG += timed-qt$${QT_MAJOR_VERSION} timed-voland-qt$${QT_MAJOR_VERSION}

isEmpty(SRCDIR) SRCDIR = "."

# D-Bus interfaces
timed.files = $$SRCDIR/com.nokia.time.xml
timed.header_flags = -N -c TimeDaemon
timed.source_flags = -N -c TimeDaemon
DBUS_INTERFACES += timed

SOURCES += $$SRCDIR/plugin.cpp \
    $$SRCDIR/alarmsbackendmodel.cpp \
    $$SRCDIR/alarmsbackendmodel_p.cpp \
    $$SRCDIR/enabledalarmsproxymodel.cpp \
    $$SRCDIR/alarmobject.cpp \
    $$SRCDIR/alarmhandlerinterface.cpp \
    $$SRCDIR/alarmdialogobject.cpp \
    $$SRCDIR/alarmsettings.cpp \
    $$SRCDIR/interface.cpp

HEADERS += $$SRCDIR/alarmsbackendmodel.h \
    $$SRCDIR/alarmsbackendmodel_p.h \
    $$SRCDIR/enabledalarmsproxymodel.h \
    $$SRCDIR/alarmobject.h \
    $$SRCDIR/alarmhandlerinterface.h \
    $$SRCDIR/alarmdialogobject.h \
    $$SRCDIR/alarmsettings.h \
    $$SRCDIR/interface.h
