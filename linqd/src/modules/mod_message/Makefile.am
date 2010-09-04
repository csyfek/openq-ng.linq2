EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_message.la

mod_message_la_SOURCES = \
	message.h \
	message.cpp

INCLUDES = -I../../include -I../../utils

mod_message_la_LDFLAGS = -module -avoid-version
mod_message_la_LIBADD = ../../utils/libutils.a
