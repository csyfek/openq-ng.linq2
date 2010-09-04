%define release 1
%define prefix  /usr
%define name	linq
%define version 2.0.0


Summary: 	LinQ Instant Messaging Client
Name:		%{name}
Version:    %{version}
Release:	%{release}
License: 	GPL
URL: 		http://myicq.cosoft.org.cn
Group: 		Applications/System
Source0:	%{name}-%{version}.tar.bz2
Packager:       Hu Zheng <huzheng_001@163.com>
BuildRoot:	%{_builddir}/%{name}-%{version}-root

Requires: gtkmm2 >= 2.2.0
Requires: liblinq >= 2.0.0

Docdir:         %{prefix}/share/doc

%description
LinQ Instant Messaging Client

%prep
%setup

%build

%configure
make

%install
rm -rf $RPM_BUILD_ROOT
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT


%files
%defattr(-, root, root)
%{_bindir}/linq
%{_datadir}/applications/linq.desktop
%{_datadir}/linq
%{_datadir}/locale/*/LC_MESSAGES/linq.mo
%{_datadir}/pixmaps/linq.png
