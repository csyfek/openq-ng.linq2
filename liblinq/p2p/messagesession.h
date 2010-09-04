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

#ifndef _MESSAGE_SESSION_H
#define _MESSAGE_SESSION_H

#include "tcpsession.h"
#include "sessionlistener.h"


class MessageSession : public TCPSessionListener {
public:
	MessageSession(TCPSessionBase *tcp, SessionListener *l);

	virtual void destroy() { delete this; }
	virtual void onReceive(uint16 cmd, const char *data, int n);
	virtual void onClose();

	uint32 sendMessage(const char *text);

private:
	void onMessage(InPacket &in);
	void onMessageAck(InPacket &in);

	TCPSessionBase *tcpSession;
	SessionListener *listener;

	static uint32 msgID;
};


#endif
