/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2003 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#ifndef _SOCKET_H
#define _SOCKET_H

#include "icqtypes.h"

#ifdef _WIN32
#include <winsock.h>
typedef int		socklen_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#endif


class SocketListener {
public:
	virtual void onSocketRead() {}
	virtual void onSocketWrite() {}
	virtual void onSocketException() {}
};


class Socket {
public:
	enum { READ, WRITE, EXCEPTION };

	virtual ~Socket() {}

	virtual int getFd() = 0;
	virtual bool create(int type, SocketListener *l) = 0;
	virtual void addEvent(int event) = 0;
	virtual void removeEvent(int event) = 0;
	virtual void connect(const char *host, uint16 port) = 0;
	virtual int receive(char *buf, int n) = 0;
	virtual int send(const char *buf, int n) = 0;
	virtual void close() = 0;
};


#endif
