EXTRA_DIST = resolver.dsp

bin_PROGRAMS = linq_resolver

linq_resolver_SOURCES = \
	resolver.h \
	resolver.cpp \
	client.h \
	client.cpp \
	option.h \
	option.cpp

INCLUDES = -DLINQ_CONFIG_DIR=\"$(LINQ_CONFIG_DIR)\" -DLINQ_LIB_DIR=\"$(LINQ_LIB_DIR)\" -I../include -I../utils
linq_resolver_LDADD = ../utils/libutils.a
