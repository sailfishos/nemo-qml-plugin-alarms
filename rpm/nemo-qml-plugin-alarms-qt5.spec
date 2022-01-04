Name:       nemo-qml-plugin-alarms-qt5

Summary:    Alarms plugin for Nemo Mobile
Version:    0.3.11
Release:    1
License:    BSD
URL:        https://github.com/sailfishos/nemo-qml-plugin-alarms
Source0:    %{name}-%{version}.tar.bz2
Requires:   timed-qt5 >= 2.88
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(Qt5Test)
BuildRequires:  pkgconfig(timed-qt5) >= 2.67

%description
%{summary}.

%package tests
Summary:    QML alarms plugin tests
Requires:   %{name} = %{version}-%{release}

%description tests
%{summary}.

%prep
%setup -q -n %{name}-%{version}

%build

%qmake5 

make %{?_smp_mflags}

%install
%qmake5_install

# org.nemomobile.alarms legacy import
mkdir -p %{buildroot}%{_libdir}/qt5/qml/org/nemomobile/alarms/
ln -sf %{_libdir}/qt5/qml/Nemo/Alarms/libnemoalarms.so %{buildroot}%{_libdir}/qt5/qml/org/nemomobile/alarms/
sed 's/Nemo.Alarms/org.nemomobile.alarms/' < src/qmldir > %{buildroot}%{_libdir}/qt5/qml/org/nemomobile/alarms/qmldir

%files
%defattr(-,root,root,-)
%license LICENSE.BSD
%dir %{_libdir}/qt5/qml/Nemo/Alarms
%{_libdir}/qt5/qml/Nemo/Alarms/libnemoalarms.so
%{_libdir}/qt5/qml/Nemo/Alarms/plugins.qmltypes
%{_libdir}/qt5/qml/Nemo/Alarms/qmldir

# org.nemomobile.alarms legacy import
%dir %{_libdir}/qt5/qml/org/nemomobile/alarms
%{_libdir}/qt5/qml/org/nemomobile/alarms/libnemoalarms.so
%{_libdir}/qt5/qml/org/nemomobile/alarms/qmldir

%files tests
%defattr(-,root,root,-)
/opt/tests/nemo-qml-plugins-qt5/alarms/*
