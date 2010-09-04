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

#include "socksudpproxy.h"
#include <assert.h>


static uint32 resolveHost(const char *host)
{
	in_addr addr;
	addr.s_addr = inet_addr(host);
	if (addr.s_addr == INADDR_NONE) {
		hostent *hent = gethostbyname(host);
		if (hent)
			addr = *(in_addr *) hent->h_addr;
	}
	return addr.s_addr;
}


enum {
	SOCKS_UDP_NOT_CONN,
	SOCKS_UDP_METHOD,
	SOCKS_UDP_AUTH,
	SOCKS_UDP_ASSOCIATE,
	SOCKS_UDP_ESTABLISHED,
};


SocksUDPProxy::SocksUDPProxy(PROXY_INFO &info)
{
	proxyInfo = info;
	
	udpSocket = new NullProxy;
	tcpSocket = new NullProxy;

	eventMask = 0;
	listener = NULL;

	status = SOCKS_UDP_NOT_CONN;
}

SocksUDPProxy::~SocksUDPProxy()
{
	delete udpSocket;
	delete tcpSocket;
}

void SocksUDPProxy::fireEvent(int event)
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

bool SocksUDPProxy::create(int type, SocketListener *l)
{
	assert(type == SOCK_DGRAM);

	listener = l;
	if (!udpSocket->create(type, l))
		return false;

	return tcpSocket->create(SOCK_STREAM, this);
}

void SocksUDPProxy::close()
{
	tcpSocket->close();
	udpSocket->close();
}

void SocksUDPProxy::addEvent(int event)
{
	eventMask |= (1 << event);

	if (status == SOCKS_UDP_ESTABLISHED)
		udpSocket->addEvent(event);
}

void SocksUDPProxy::removeEvent(int event)
{
	eventMask &= ~(1 << event);

	if (status == SOCKS_UDP_ESTABLISHED)
		udpSocket->removeEvent(event);
}

void SocksUDPProxy::connect(const char *host, uint16 port)
{
	destHost = host;
	destPort = port;

	tcpSocket->addEvent(WRITE);
	tcpSocket->addEvent(EXCEPTION);
	tcpSocket->connect(proxyInfo.host.c_str(), proxyInfo.port);
}

int SocksUDPProxy::send(const char *data, int n)
{
	char buf[4096];
	char *p = buf;

	*(uint16 *) p = 0;
	p += 2;
	*p++ = 0;

	bool resolve = proxyInfo.resolve;
	if (resolve) {
		*p++ = 1;		// IPv4
		uint32 ip = resolveHost(destHost.c_str());
		if (ip == INADDR_NONE)
			resolve = false;
		else {
			*(uint32 *) p = ip;
			p += 4;
		}
	}
	if (!resolve) {
		*p++ = 3;		// Domain name
		uint8 len = destHost.length();
		*p++ = len;
		memcpy(p, destHost.c_str(), len);
		p += len;
	}
	*(uint16 *) p = htons(destPort);
	p += 2;

	int len = p - buf + n;
	if (len > sizeof(buf))
		return -1;

	memcpy(p, data, n);
	len = udpSocket->send(buf, len);
	if (len < (p - buf))
		return -1;

	return (len - (p - buf));
}

int SocksUDPProxy::receive(char *data, int n)
{
	char buf[1024];
	int len = udpSocket->receive(buf, sizeof(buf));

	len -= 10;
	if (len < 0 || n < len)
		return -1;

	memcpy(data, buf + 10, len);
	return len;
}

bool SocksUDPProxy::onStatusMethod(const char *data, int n)
{
	if (n != 2 || data[0] != 5 || (data[1] != 0 && data[1] != 2))
		return false;

	if (data[1] == 0)
		status = SOCKS_UDP_ASSOCIATE;
	else {
		status = SOCKS_UDP_AUTH;

		char buf[128];
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

bool SocksUDPProxy::onStatusAuth(const char *data, int n)
{
	if (n != 2 || data[0] != 1 || data[1] != 0)
		return false;

	status = SOCKS_UDP_ASSOCIATE;
	return true;
}

bool SocksUDPProxy::onStatusAssociate(const char *data, int n)
{
	if (n != 10 || data[0] != 5 || data[1] != 0 || data[3] != 1)
		return false;

	status = SOCKS_UDP_ESTABLISHED;

	uint32 ip = ntohl(*(uint32 *) &data[4]);
	uint16 port = ntohs(*(uint16 *) &data[8]);
	udpSocket->connect(ip, port);

	fireEvent(WRITE);
	return true;
}

void SocksUDPProxy::onSocketWrite()
{
	tcpSocket->removeEvent(WRITE);
	tcpSocket->addEvent(READ);

	char buf[3];
	buf[0] = 5;
	buf[1] = 1;
	buf[2] = (proxyInfo.user.empty() ? 0 : 2);
	tcpSocket->send(buf, 3);

	status = SOCKS_UDP_METHOD;
}

void SocksUDPProxy::onSocketException()
{
	fireEvent(EXCEPTION);
}

void SocksUDPProxy::onSocketRead()
{
	char buf[256];
	int n = tcpSocket->receive(buf, sizeof(buf));

	bool ret = false;

	switch (status) {
	case SOCKS_UDP_METHOD:
		ret = onStatusMethod(buf, n);
		break;
	case SOCKS_UDP_AUTH:
		ret = onStatusAuth(buf, n);
		break;
	case SOCKS_UDP_ASSOCIATE:
		ret = onStatusAssociate(buf, n);
		break;
	}

	if (!ret) {
		fireEvent(EXCEPTION);
		return;
	}

	if (status == SOCKS_UDP_ASSOCIATE) {
		sockaddr_in addr;
		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = 0;

		int fd = udpSocket->getFd();
		bind(fd, (sockaddr *) &addr, sizeof(addr));

		socklen_t len = sizeof(addr);
		getsockname(fd, (sockaddr *) &addr, &len);

		buf[0] = 5;
		buf[1] = 3;
		buf[2] = 0;
		buf[3] = 1;
		*(in_addr *) &buf[4] = addr.sin_addr;
		*(uint16 *) &buf[8] = addr.sin_port;
		tcpSocket->send(buf, 10);
	}
}
