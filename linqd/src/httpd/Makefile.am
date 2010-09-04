EXTRA_DIST = httpd.dsp

bin_PROGRAMS = linq_httpd

linq_httpd_SOURCES = \
	https.h \
	https.cpp \
	option.h \
	option.cpp \
	session.h \
	session.cpp \
	udpsocket.h \
	udpsocket.cpp

INCLUDES = -DLINQ_CONFIG_DIR=\"$(LINQ_CONFIG_DIR)\" -DLINQ_LIB_DIR=\"$(LINQ_LIB_DIR)\" -I../include -I../utils
linq_httpd_LDADD = ../utils/libutils.a
