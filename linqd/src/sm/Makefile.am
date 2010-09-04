EXTRA_DIST = sm.dsp

bin_PROGRAMS = linq_sm

linq_sm_SOURCES = \
	client.cpp \
	option.cpp \
	session.cpp \
	sessionhash.cpp \
	sessionmanager.cpp \
	s2s.cpp \
	client.h \
	option.h \
	session.h \
	sessionhash.h \
	sessionmanager.h \
	s2s.h

INCLUDES = -DLINQ_CONFIG_DIR=\"$(LINQ_CONFIG_DIR)\" -DLINQ_LIB_DIR=\"$(LINQ_LIB_DIR)\" -I../include -I../utils
linq_sm_LDADD = ../utils/libutils.a -ldl
