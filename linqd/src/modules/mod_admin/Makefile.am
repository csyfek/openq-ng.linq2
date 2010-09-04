EXTRA_DIST = *.dsp

pkgdir = $(libdir)/linqd/modules

pkg_LTLIBRARIES = mod_admin.la

mod_admin_la_SOURCES = \
	admin.h \
	admin.cpp \
	client.h \
	client.cpp

INCLUDES = -I../../include -I../../utils

mod_admin_la_LDFLAGS = -module -avoid-version
mod_admin_la_LIBADD = ../../utils/libutils.a
