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

#include "sockstcpproxy.h"
#include "nullproxy.h"
#include <assert.h>


enum {
	SOCKS_TCP_NOT_CONN,
	SOCKS_TCP_METHOD,
	SOCKS_TCP_AUTH,
	SOCKS_TCP_CONNECT,
	SOCKS_TCP_ESTABLISHED,
};


SocksTCPProxy::SocksTCPProxy(PROXY_INFO &info)
{
	proxyInfo = info;

	tcpSocket = new NullProxy;

	eventMask = 0;
	listener = NULL;

	status = SOCKS_TCP_NOT_CONN;
}

SocksTCPProxy::~SocksTCPProxy()
{
	delete tcpSocket;
}

void SocksTCPProxy::fireEvent(int event)
{
	if (!listener || ((1 << event) & eventMask) == 0)
		return;

	if (event == READ)
		listener->onSocketRead();
	else if (event == WRITE)
		listener->onSocketWrite();
	else if (event == EXCEPTION) {
		close();
		listener->onSocketException();
	}
}

void SocksTCPProxy::close()
{
	tcpSocket->close();
}

bool SocksTCPProxy::create(int type, SocketListener *l)
{
	assert(type == SOCK_STREAM);

	listener = l;
	return tcpSocket->create(type, this);
}

void SocksTCPProxy::addEvent(int event)
{
	eventMask |= (1 << event);

	if (status == SOCKS_TCP_ESTABLISHED)
		tcpSocket->addEvent(event);
}

void SocksTCPProxy::removeEvent(int event)
{
	eventMask &= ~(1 << event);

	if (status == SOCKS_TCP_ESTABLISHED)
		tcpSocket->removeEvent(event);
}

void SocksTCPProxy::connect(const char *host, uint16 port)
{
	destHost = host;
	destPort = port;

	tcpSocket->addEvent(WRITE);
	tcpSocket->addEvent(EXCEPTION);
	tcpSocket->connect(proxyInfo.host.c_str(), proxyInfo.port);
}

int SocksTCPProxy::send(const char *buf, int n)
{
	if (status != SOCKS_TCP_ESTABLISHED)
		return -1;

	return tcpSocket->send(buf, n);
}

int SocksTCPProxy::receive(char *buf, int n)
{
	if (status != SOCKS_TCP_ESTABLISHED)
		return -1;

	return tcpSocket->receive(buf, n);
}

void SocksTCPProxy::onSocketWrite()
{
	if (status == SOCKS_TCP_ESTABLISHED) {
		fireEvent(WRITE);
		return;
	}

	tcpSocket->removeEvent(WRITE);
	tcpSocket->addEvent(READ);

	status = SOCKS_TCP_METHOD;

	char buf[3];
	buf[0] = 5;
	buf[1] = 1;
	buf[2] = (proxyInfo.user.empty() ? 0 : 2);
	tcpSocket->send(buf, 3);
}

void SocksTCPProxy::onSocketException()
{
	fireEvent(EXCEPTION);
}

bool SocksTCPProxy::onStatusMethod(const char *data, int n)
{
	if (n != 2 || data[0] != 5 || (data[1] != 0 && data[1] != 2))
		return false;

	if (data[1] == 0)
		status = SOCKS_TCP_CONNECT;
	else {
		status = SOCKS_TCP_AUTH;

		char buf[256];
		char *p = buf;

		*p++ = 1;
		*p++ = n = proxyInfo.user.length();
		memcpy(p, proxyInfo.user.c_str(), n);
		p += n;
		*p++ = n = proxyInfo.passwd.length();
		memcpy(p, proxyInfo.passwd.c_str(), n);
		p += n;

		tcpSocket->send(buf, p - buf);
	}
	return true;
}

bool SocksTCPProxy::onStatusAuth(const char *data, int n)
{
	if (n != 2 || data[0] != 1 || data[1] != 0)
		return false;

	status = SOCKS_TCP_CONNECT;
	return true;
}

bool SocksTCPProxy::onStatusConnect(const char *data, int n)
{
	if (n != 10 || data[0] != 5 || data[1] != 0)
		return false;

	status = SOCKS_TCP_ESTABLISHED;

	fireEvent(WRITE);
	return true;
}

void SocksTCPProxy::onSocketRead()
{
	if (status == SOCKS_TCP_ESTABLISHED) {
		fireEvent(READ);
		return;
	}

	char buf[1024];
	int n = tcpSocket->receive(buf, sizeof(buf));

	bool ret = false;

	switch (status) {
	case SOCKS_TCP_METHOD:
		ret = onStatusMethod(buf, n);
		break;
	case SOCKS_TCP_AUTH:
		ret = onStatusAuth(buf, n);
		break;
	case SOCKS_TCP_CONNECT:
		ret = onStatusConnect(buf, n);
		break;
	}

	if (!ret) {
		fireEvent(EXCEPTION);
		return;
	}

	if (status == SOCKS_TCP_CONNECT) {
		char buf[10];

		buf[0] = 5;
		buf[1] = 1;
		buf[2] = 0;
		buf[3] = 1;

		*(uint32 *) &buf[4] = inet_addr(destHost.c_str());
		*(uint16 *) &buf[8] = htons(destPort);

		tcpSocket->send(buf, 10);
	}
}
