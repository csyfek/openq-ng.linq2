SUBDIRS = linux win32

noinst_LIBRARIES = libutils.a

libutils_a_SOURCES = \
	debug.h \
	sync.h \
	fdset.h \
	list.h \
	list.cpp \
	sqlstmt.h \
	sqlstmt.cpp \
	packet.h \
	packet.cpp \
	socket.h \
	socket.cpp \
	dllmodule.h \
	tcpstream.h \
	tcpstream.cpp \
	thread.h \
	workerthread.h \
	workerthread.cpp \
	profileimpl.h \
	profileimpl.cpp \
	loggerimpl.h \
	loggerimpl.cpp \
	poolimpl.h \
	poolimpl.cpp \
	dbmodule.h \
	dbmodule.cpp \
	configparser.h \
	configparser.cpp \
	servicemain.h \
	servicemain.cpp

EXTRA_DIST = *.dsp poll.cpp select.cpp

INCLUDES = -I../include -Ilinux

libutils_a_LIBADD = linux/*.o
