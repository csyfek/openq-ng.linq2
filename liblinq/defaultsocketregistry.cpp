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

#define FD_SETSIZE		1024

#include "defaultsocketregistry.h"
#include "icqlog.h"
#include <assert.h>


DefaultSocketRegistry::DefaultSocketRegistry()
{
}

DefaultSocketRegistry::~DefaultSocketRegistry()
{
	map<int, FD_ENTRY *>::iterator it;
	for (it = socketMap.begin(); it != socketMap.end(); ++it)
		delete (*it).second;
}

void DefaultSocketRegistry::add(int fd, SocketListener *l)
{
	map<int, FD_ENTRY *>::iterator it = socketMap.find(fd);

	assert(it == socketMap.end());

	FD_ENTRY *e = new FD_ENTRY;
	e->events = 0;
	e->listener = l;
	socketMap[fd] = e;
}

void DefaultSocketRegistry::remove(int fd)
{
	map<int, FD_ENTRY *>::iterator it = socketMap.find(fd);
	assert(it != socketMap.end());

	delete (*it).second;
	socketMap.erase(it);
}

void DefaultSocketRegistry::addEvent(int fd, int event)
{
	map<int, FD_ENTRY *>::iterator it = socketMap.find(fd);
	assert(it != socketMap.end());

	FD_ENTRY *e = (*it).second;
	e->events |= (1 << event);
}

void DefaultSocketRegistry::removeEvent(int fd, int event)
{
	map<int, FD_ENTRY *>::iterator it = socketMap.find(fd);
	assert(it != socketMap.end());

	FD_ENTRY *e = (*it).second;
	e->events &= ~(1 << event);
}

bool DefaultSocketRegistry::poll()
{
	fd_set readfds, writefds, exceptfds;
	int maxfd = -1;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
	FD_ZERO(&exceptfds);

	map<int, FD_ENTRY *>::iterator it, next;

	for (it = socketMap.begin(); it != socketMap.end(); ++it) {
		int fd = (*it).first;
		FD_ENTRY *e = (*it).second;

		if (e->events & (1 << Socket::READ))
			FD_SET(fd, &readfds);
		if (e->events & (1 << Socket::WRITE))
			FD_SET(fd, &writefds);
		if (e->events & (1 << Socket::EXCEPTION))
			FD_SET(fd, &exceptfds);

		if (fd > maxfd)
			maxfd = fd;
	}

#ifdef _WIN32
	if (maxfd < 0)
		return true;
#endif

	timeval tv = { 0, 0 };
	int n = select(maxfd + 1, &readfds, &writefds, &exceptfds, &tv);
	if (n < 0) {
		ICQ_LOG("select() failed: \n");
		return false;
	}

	for (it = socketMap.begin(); it != socketMap.end(); it = next) {
		next = it;
		++next;

		int fd = (*it).first;
		FD_ENTRY *e = (*it).second;

		if (FD_ISSET(fd, &readfds))
			e->listener->onSocketRead();
		if (FD_ISSET(fd, &writefds))
			e->listener->onSocketWrite();
		if (FD_ISSET(fd, &exceptfds))
			e->listener->onSocketException();
	}

	return true;
}
