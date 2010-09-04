EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_presence.la

mod_presence_la_SOURCES = \
	presence.h \
	presence.cpp \
	stringset.h \
	stringset.cpp

INCLUDES = -I../../include -I../../utils

mod_presence_la_LDFLAGS = -module -avoid-version
mod_presence_la_LIBADD = ../../utils/libutils.a
