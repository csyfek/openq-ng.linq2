Name: linqd
Version: 2.0.0
Release: a1
Source0:	%{name}-%{version}.tar.bz2
URL: http://myicq.cosoft.org.cn
BuildRoot: %{_tmppath}/%{name}-%{version}-root
Summary: The LinQ server.
License: Close Source
Group: Applications/Miscellaneous
Prereq: /sbin/ldconfig, /sbin/install-info, grep,  fileutils, chkconfig
BuildRequires: gcc-c++
BuildRequires: libtool automake autoconf
Requires: bash


%description
linqd is the server program of LinQ Instant Messaging system.

%prep
%setup -q 

%build


%configure
make

%install
rm -rf $RPM_BUILD_ROOT

mkdir -p $RPM_BUILD_ROOT/etc/linqd
mkdir -p $RPM_BUILD_ROOT/etc/linqd/install

mkdir -p $RPM_BUILD_ROOT/usr/lib/linqd/db
mkdir -p $RPM_BUILD_ROOT/usr/lib/linqd/modules

mkdir -p $RPM_BUILD_ROOT/usr/bin

install -m 0644 INSTALL $RPM_BUILD_ROOT/etc/linqd/install
install -m 0644 linq.sql $RPM_BUILD_ROOT/etc/linqd/install

install -m 0600 etc/c2s.conf $RPM_BUILD_ROOT/etc/linqd/
install -m 0600 etc/httpd.conf $RPM_BUILD_ROOT/etc/linqd/
install -m 0600 etc/linqd.conf $RPM_BUILD_ROOT/etc/linqd/
install -m 0600 etc/logd.conf $RPM_BUILD_ROOT/etc/linqd/
install -m 0600 etc/modules.conf $RPM_BUILD_ROOT/etc/linqd/
install -m 0600 etc/resolver.conf $RPM_BUILD_ROOT/etc/linqd/
install -m 0600 etc/s2s.conf $RPM_BUILD_ROOT/etc/linqd/
install -m 0600 etc/sm.conf $RPM_BUILD_ROOT/etc/linqd/

install -m 0755 src/c2s/linq_c2s $RPM_BUILD_ROOT/usr/bin
install -m 0755 src/httpd/linq_httpd $RPM_BUILD_ROOT/usr/bin
install -m 0755 src/linqd/linqd $RPM_BUILD_ROOT/usr/bin
install -m 0755 src/logd/linq_logd $RPM_BUILD_ROOT/usr/bin
install -m 0755 src/resolver/linq_resolver $RPM_BUILD_ROOT/usr/bin
install -m 0755 src/s2s/linq_s2s $RPM_BUILD_ROOT/usr/bin
install -m 0755 src/sm/linq_sm $RPM_BUILD_ROOT/usr/bin

install -m 0755 src/db/.libs/db_mysql.so $RPM_BUILD_ROOT/usr/lib/linqd/db

install -m 0755 src/modules/mod_admin/.libs/mod_admin.so $RPM_BUILD_ROOT/usr/lib/linqd/modules
install -m 0755 src/modules/mod_log/.libs/mod_log.so $RPM_BUILD_ROOT/usr/lib/linqd/modules
install -m 0755 src/modules/mod_message/.libs/mod_message.so $RPM_BUILD_ROOT/usr/lib/linqd/modules
install -m 0755 src/modules/mod_offline/.libs/mod_offline.so $RPM_BUILD_ROOT/usr/lib/linqd/modules
install -m 0755 src/modules/mod_presence/.libs/mod_presence.so $RPM_BUILD_ROOT/usr/lib/linqd/modules
install -m 0755 src/modules/mod_roster/.libs/mod_roster.so $RPM_BUILD_ROOT/usr/lib/linqd/modules
install -m 0755 src/modules/mod_search/.libs/mod_search.so $RPM_BUILD_ROOT/usr/lib/linqd/modules
install -m 0755 src/modules/mod_vcard/.libs/mod_vcard.so $RPM_BUILD_ROOT/usr/lib/linqd/modules

mkdir -p $RPM_BUILD_ROOT/etc/rc.d/init.d
mkdir -p $RPM_BUILD_ROOT/var/run/linqd
install -m 0755 linqd.init $RPM_BUILD_ROOT/etc/rc.d/init.d/linqd

%clean
rm -rf $RPM_BUILD_ROOT

%post
/sbin/chkconfig --add linqd
echo "Please read /etc/linqd/install/INSTALL to complete the installation."

%preun
    /sbin/chkconfig --del linqd


%files
%defattr(-,root,root)

/usr/bin/*
%config(noreplace) /etc/linqd/*.conf
/etc/linqd
/etc/rc.d/init.d/linqd
/usr/lib/linqd
%dir /var/run/linqd


%changelog
* Mon Jul 07 2003 Hu Zheng <huzheng_001@163.com> 2.0.0-a1
- First public alpha release of linqd.
