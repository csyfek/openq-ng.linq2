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

#ifndef _HTTP_PROXY_H
#define _HTTP_PROXY_H

#include "socket.h"
#include "proxy.h"


class HTTPProxy : public Socket, public SocketListener {
public:
	HTTPProxy(PROXY_INFO &info);
	virtual ~HTTPProxy();

	virtual int getFd() { return tcpSocket->getFd(); }
	virtual void onSocketRead();
	virtual void onSocketWrite();
	virtual void onSocketException();

	virtual bool create(int type, SocketListener *l);
	virtual void addEvent(int event);
	virtual void removeEvent(int event);
	virtual void connect(const char *host, uint16 port);
	virtual int receive(char *buf, int n);
	virtual int send(const char *buf, int n);
	virtual void close() { tcpSocket->close(); }

private:
	void fireEvent(int event);
	void recvPacket();

	Socket *tcpSocket;
	SocketListener *listener;
	int eventMask;
	int status;

	PROXY_INFO proxyInfo;
	string destHost;

	char buffer[4096];
	int bufSize;
};


#endif
