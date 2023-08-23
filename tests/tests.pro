PACKAGENAME = nemo-qml-plugin-alarms-qt$${QT_MAJOR_VERSION}
INSTALLLOCATION = /opt/tests/nemo-qml-plugins-qt$${QT_MAJOR_VERSION}/alarms

TEMPLATE = subdirs
SUBDIRS = tst_alarmsbackendmodel \
    tst_alarmhandler

tests_xml.target = tests.xml
tests_xml.depends = $$PWD/tests.xml.in
tests_xml.commands = sed -e "\"s:@PACKAGENAME@:$${PACKAGENAME}:g;s:@INSTALLLOCATION@:$${INSTALLLOCATION}:g\"" $< > $@

QMAKE_EXTRA_TARGETS = tests_xml
QMAKE_CLEAN += $$tests_xml.target
PRE_TARGETDEPS += $$tests_xml.target

tests_install.depends = tests_xml
tests_install.files = tests.xml
tests_install.path = $${INSTALLLOCATION}
tests_install.CONFIG += no_check_exist

INSTALLS = tests_install
