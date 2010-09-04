EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_vcard.la

mod_vcard_la_SOURCES = \
	vcard.h \
	vcard.cpp

INCLUDES = -I../../include -I../../utils

mod_vcard_la_LDFLAGS = -module -avoid-version
mod_vcard_la_LIBADD = ../../utils/libutils.a
