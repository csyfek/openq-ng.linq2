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

#ifndef _SOCKET_REGISTRY_H
#define _SOCKET_REGISTRY_H

#include "socket.h"


class SocketRegistry {
public:
	virtual void add(int fd, SocketListener *l) = 0;
	virtual void remove(int fd) = 0;
	virtual void addEvent(int fd, int event) = 0;
	virtual void removeEvent(int fd, int event) = 0;
};


#endif
