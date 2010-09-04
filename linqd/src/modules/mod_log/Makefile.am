EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_log.la

mod_log_la_SOURCES = \
	log.h \
	log.cpp

INCLUDES = -I../../include -I../../utils

mod_log_la_LDFLAGS = -module -avoid-version
mod_log_la_LIBADD = ../../utils/libutils.a
