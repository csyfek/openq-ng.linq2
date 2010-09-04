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

#ifndef _SOCKS_TCP_PROXY_H
#define _SOCKS_TCP_PROXY_H

#include "socket.h"
#include "proxy.h"


class SocksTCPProxy : public Socket, public SocketListener {
public:
	SocksTCPProxy(PROXY_INFO &info);
	virtual ~SocksTCPProxy();

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
	virtual void close();

private:
	void fireEvent(int event);

	bool onStatusMethod(const char *data, int n);
	bool onStatusAuth(const char *data, int n);
	bool onStatusConnect(const char *data, int n);

	Socket *tcpSocket;

	PROXY_INFO proxyInfo;
	string destHost;
	uint16 destPort;
	int eventMask;
	SocketListener *listener;
	int status;
};


#endif
