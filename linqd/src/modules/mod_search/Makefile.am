EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_search.la

mod_search_la_SOURCES = \
	search.h \
	search.cpp \
	sessionmanager.h \
	sessionmanager.cpp

INCLUDES = -I../../include -I../../utils

mod_search_la_LDFLAGS = -module -avoid-version
mod_search_la_LIBADD = ../../utils/libutils.a
