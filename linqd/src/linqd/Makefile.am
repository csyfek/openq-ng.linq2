EXTRA_DIST = myicqd.dsp

bin_PROGRAMS = linqd

linqd_SOURCES = \
	icqmain.h \
	icqmain.cpp \
	c2s.h \
	c2s.cpp \
	module.h \
	module.cpp \
	modulelist.h \
	modulelist.cpp \
	option.h \
	option.cpp \
	eventthread.h \
	eventthread.cpp \
	clientsession.h \
	clientsession.cpp \
	sessionhash.h \
	sessionhash.cpp

INCLUDES = -DLINQ_CONFIG_DIR=\"$(LINQ_CONFIG_DIR)\" -DLINQ_LIB_DIR=\"$(LINQ_LIB_DIR)\" -I../include -I../utils
linqd_LDADD = ../utils/libutils.a -ldl -lpthread
