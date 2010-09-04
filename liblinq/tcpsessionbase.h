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

#ifndef _TCP_SESSION_BASE_H
#define _TCP_SESSION_BASE_H

#include "icqtypes.h"


class TCPSessionBase {
public:
	virtual void destroy() = 0;

	virtual const char *getContactName() = 0;
	virtual bool isSender() = 0;
	virtual void enableWrite(bool enable) = 0;
	virtual bool sendPacket(uint16 cmd, const char *data, int n) = 0;
};


class TCPSessionListener {
public:
	virtual void destroy() = 0;

	virtual void onReceive(uint16 cmd, const char *data, int n) {}
	virtual void onSend() {}
	virtual void onClose() {}
};


#endif
