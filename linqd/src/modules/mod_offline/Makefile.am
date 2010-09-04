EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_offline.la

mod_offline_la_SOURCES = \
	offline.h \
	offline.cpp

INCLUDES = -I../../include -I../../utils

mod_offline_la_LDFLAGS = -module -avoid-version
mod_offline_la_LIBADD = ../../utils/libutils.a
