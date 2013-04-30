TEMPLATE = subdirs
SUBDIRS = tst_alarmsbackendmodel \
    tst_alarmhandler

tests_xml.target = tests.xml
tests_xml.files = tests.xml
equals(QT_MAJOR_VERSION, 4): tests_xml.path = /opt/tests/nemo-qml-plugins/alarms
equals(QT_MAJOR_VERSION, 5): tests_xml.path = /opt/tests/nemo-qml-plugins-qt5/alarms
INSTALLS += tests_xml
