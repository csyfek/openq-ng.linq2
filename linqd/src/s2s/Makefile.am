EXTRA_DIST = *.dsp

bin_PROGRAMS = linq_s2s

linq_s2s_SOURCES = \
	listensocket.cpp \
	resolver.cpp \
	option.cpp \
	s2s.cpp \
	server.cpp \
	serverhash.cpp \
	sessionmanager.cpp \
	listensocket.h \
	resolver.h \
	option.h \
	s2s.h \
	server.h \
	serverhash.h \
	sessionmanager.h

INCLUDES = -DLINQ_CONFIG_DIR=\"$(LINQ_CONFIG_DIR)\" -DLINQ_LIB_DIR=\"$(LINQ_LIB_DIR)\" -I../include -I../utils

linq_s2s_LDADD = ../utils/libutils.a
