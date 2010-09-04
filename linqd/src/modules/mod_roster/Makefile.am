EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_roster.la

mod_roster_la_SOURCES = \
	roster.h \
	roster.cpp

INCLUDES = -I../../include -I../../utils

mod_roster_la_LDFLAGS = -module -avoid-version
mod_roster_la_LIBADD = ../../utils/libutils.a
