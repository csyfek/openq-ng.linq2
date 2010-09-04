EXTRA_DIST = c2s.dsp

bin_PROGRAMS = linq_c2s

linq_c2s_SOURCES = \
	c2s.h \
	c2s.cpp \
	option.h \
	option.cpp \
	session.h \
	session.cpp \
	sessionhash.h \
	sessionhash.cpp \
	realserver.h \
	realserver.cpp

INCLUDES = -DLINQ_CONFIG_DIR=\"$(LINQ_CONFIG_DIR)\" -DLINQ_LIB_DIR=\"$(LINQ_LIB_DIR)\" -I../include -I../utils

linq_c2s_LDADD = ../utils/libutils.a -ldl
