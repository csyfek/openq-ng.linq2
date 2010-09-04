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

#ifndef _DEFAULT_SOCKET_REGISTRY_H
#define _DEFAULT_SOCKET_REGISTRY_H

#pragma warning(disable:4786)

#include "socketregistry.h"
#include <map>

using namespace std;


struct FD_ENTRY {
	int events;
	SocketListener *listener;
};


class DefaultSocketRegistry : public SocketRegistry {
public:
	DefaultSocketRegistry();
	~DefaultSocketRegistry();

	virtual void add(int fd, SocketListener *l);
	virtual void remove(int fd);
	virtual void addEvent(int fd, int event);
	virtual void removeEvent(int fd, int event);

	bool poll();

private:
	map<int, FD_ENTRY *> socketMap;
};


#endif
