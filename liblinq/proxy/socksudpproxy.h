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

#ifndef _SOCKS_UDP_PROXY_H
#define _SOCKS_UDP_PROXY_H

#include "proxy.h"
#include "nullproxy.h"


class SocksUDPProxy : public Socket, public SocketListener {
public:
	SocksUDPProxy(PROXY_INFO &info);
	virtual ~SocksUDPProxy();

	virtual int getFd() { return udpSocket->getFd(); }
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
	bool onStatusAssociate(const char *data, int n);

	PROXY_INFO proxyInfo;
	string destHost;
	uint16 destPort;

	NullProxy *udpSocket;
	NullProxy *tcpSocket;
	int status;

	int eventMask;
	SocketListener *listener;
};


#endif
