EXTRA_DIST = logd.dsp

bin_PROGRAMS = linq_logd

linq_logd_SOURCES = \
	logapp.h \
	logapp.cpp \
	logger.h \
	logger.cpp \
	client.h \
	client.cpp \
	option.h \
	option.cpp

INCLUDES = -DLINQ_CONFIG_DIR=\"$(LINQ_CONFIG_DIR)\" -DLINQ_LIB_DIR=\"$(LINQ_LIB_DIR)\" -I../include -I../utils
linq_logd_LDADD = ../utils/libutils.a
