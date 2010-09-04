EXTRA_DIST = *.dsp db_odbc.cpp

pkgdir = $(libdir)/linqd/db

pkg_LTLIBRARIES = db_mysql.la

db_mysql_la_SOURCES = db_mysql.cpp

INCLUDES = -I../include -I../utils -I/usr/include/mysql

db_mysql_la_LDFLAGS = -module -avoid-version -L/usr/lib/mysql
db_mysql_la_LIBADD = -lmysqlclient
