TEMPLATE = lib
VERSION = 2.0.0

CONFIG += staticlib release warn_on
CONFIG -= qt


HEADERS = \
	defaultsocketregistry.h \
	icqlog.h \
	icqmain.h \
	icqtypes.h \
	icqutils.h \
	md5.h \
	nullproxy.h \
	packet.h \
	proxy.h \
	sessionlistener.h \
	socket.h \
	socketregistry.h \
	tcpsession.h \
	tcpsessionbase.h \
	textstream.h \
	udppacket.h \
	udpsession.h


SOURCES = \
	defaultsocketregistry.cpp \
	icqmain.cpp \
	icqutils.cpp \
	md5.cpp \
	nullproxy.cpp \
	packet.cpp \
	tcpsession.cpp \
	textstream.cpp \
	udppacket.cpp \
	udpsession.cpp


P2P_DIR = p2p
PROXY_DIR = proxy

include($$P2P_DIR/p2p.pri)
include($$PROXY_DIR/proxy.pri)

INCLUDEPATH += .;$$P2P_DIR;$$PROXY_DIR

debug:DEFINES += _DEBUG

unix:TARGET = linq
