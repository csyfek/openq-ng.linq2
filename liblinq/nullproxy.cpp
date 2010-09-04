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

#include "nullproxy.h"
#include "socketregistry.h"
#include <string.h>
#include <assert.h>


extern SocketRegistry *socketRegistry;


NullProxy::NullProxy(int fd)
{
	sockfd = fd;

	eventMask = 0;
	listener = NULL;
}

NullProxy::~NullProxy()
{
	close();
}

void NullProxy::fireEvent(int event)
{
	if (!listener || ((1 << event) & eventMask) == 0)
		return;

	if (event == READ)
		listener->onSocketRead();
	else if (event == WRITE)
		listener->onSocketWrite();
	else if (event == EXCEPTION)
		listener->onSocketException();
}

bool NullProxy::create(int type, SocketListener *l)
{
	listener = l;

	if (sockfd < 0)
		sockfd = socket(AF_INET, type, 0);

	assert(sockfd >= 0);

	unsigned long on = 1;
#ifdef _WIN32
	if (ioctlsocket(sockfd, FIONBIO, &on) < 0)
#else
	if (ioctl(sockfd, FIONBIO, &on) < 0)
#endif
		return false;

	socketRegistry->add(sockfd, l);
	return true;
}

void NullProxy::close()
{
	if (sockfd >= 0) {
		socketRegistry->remove(sockfd);
#ifdef _WIN32
		closesocket(sockfd);
#else
		::close(sockfd);
#endif
	}
}

void NullProxy::addEvent(int event)
{
	eventMask |= (1 << event);
	socketRegistry->addEvent(sockfd, event);
}

void NullProxy::removeEvent(int event)
{
	eventMask &= ~(1 << event);
	socketRegistry->removeEvent(sockfd, event);
}

void NullProxy::connect(const char *host, uint16 port)
{
	in_addr addr;
	addr.s_addr = inet_addr(host);

	if (addr.s_addr == INADDR_NONE) {
		hostent *hent = gethostbyname(host);
		if (!hent) {
			fireEvent(EXCEPTION);
			return;
		}
		addr = *(in_addr *) hent->h_addr;
	}

	connect(ntohl(addr.s_addr), port);
}

void NullProxy::connect(uint32 ip, int port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(ip);
	addr.sin_port = htons(port);

	if (::connect(sockfd, (sockaddr *) &addr, sizeof(addr)) == 0)
		fireEvent(WRITE);
#ifdef _WIN32
	else if (WSAGetLastError() != WSAEWOULDBLOCK)
#else
	else if (errno != EINPROGRESS)
#endif
		fireEvent(EXCEPTION);
}

int NullProxy::send(const char *buf, int n)
{
	return ::send(sockfd, buf, n, 0);
}

int NullProxy::receive(char *buf, int n)
{
	return ::recv(sockfd, buf, n, 0);
}
