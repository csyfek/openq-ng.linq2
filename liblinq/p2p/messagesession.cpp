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

#include "messagesession.h"


enum {
	TCP_CMD_MESSAGE = 0x1000,
	TCP_CMD_MESSAGE_ACK,
};


uint32 MessageSession::msgID = 0xFfFfFfFf;


MessageSession::MessageSession(TCPSessionBase *tcp, SessionListener *l)
{
	tcpSession = tcp;
	listener = l;
}

uint32 MessageSession::sendMessage(const char *text)
{
	OutPacket out;
	out << --msgID << text;
	tcpSession->sendPacket(TCP_CMD_MESSAGE, out.data, out.getLength());

	return msgID;
}

void MessageSession::onClose()
{
	tcpSession->destroy();
}

void MessageSession::onReceive(uint16 cmd, const char *data, int n)
{
	InPacket in(data, n);
	
	switch (cmd) {
	case TCP_CMD_MESSAGE:
		onMessage(in);
		break;

	case TCP_CMD_MESSAGE_ACK:
		onMessageAck(in);
		break;
	}
}

void MessageSession::onMessage(InPacket &in)
{
	uint32 id;
	in >> id;

	OutPacket out;
	out << id;
	tcpSession->sendPacket(TCP_CMD_MESSAGE_ACK, out.data, out.getLength());

	ICQ_MSG msg;
	in >> msg.text;

	msg.type = MSG_TEXT;
	msg.contact = tcpSession->getContactName();
	msg.when = time(NULL);
	msg.flags = MF_FROM;

	listener->onRecvMessage(msg);
}

void MessageSession::onMessageAck(InPacket &in)
{
	uint32 id;
	in >> id;

	listener->onAck(id);
}
