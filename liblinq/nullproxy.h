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

#ifndef _NULL_PROXY_H
#define _NULL_PROXY_H

#include "socket.h"


class NullProxy : public Socket {
public:
	NullProxy(int fd = -1);
	virtual ~NullProxy();

	virtual int getFd() { return sockfd; }
	virtual bool create(int type, SocketListener *l);
	virtual void addEvent(int event);
	virtual void removeEvent(int event);
	virtual void connect(const char *host, uint16 port);
	virtual int receive(char *buf, int n);
	virtual int send(const char *buf, int n);
	virtual void close();

	void connect(uint32 ip, int port);

private:
	void fireEvent(int event);

	SocketListener *listener;
	int eventMask;
	int sockfd;
};


#endif
