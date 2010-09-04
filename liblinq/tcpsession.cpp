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

#include "tcpsession.h"
#include "icqmain.h"
#include "icqlog.h"
#include "icqutils.h"


enum {
	TCP_STATUS_NOT_CONN,
	TCP_STATUS_HELLO_WAIT,
	TCP_STATUS_ESTABLISHED,
	TCP_STATUS_LISTEN,
};

#pragma pack(1)
struct TCP_PACKET_HDR {
	uint16 ver;
	uint32 reserved;
	uint16 cmd;
};
#pragma pack()


#define TCP_CMD_HELLO	1

const uint16 TCP_VER = 1;


TCPSession::TCPSession(ICQMain *main, Socket *sock, const char *type, const char *name, bool isSend)
{
	icqMain = main;
	tcpSocket = sock;
	sessionType = type;
	contactName = name;
	this->isSend = isSend;

	tcpSocket->create(SOCK_STREAM, this);
	tcpSocket->addEvent(Socket::READ);

	listener = NULL;
	writeEnabled = false;
	isDisabled = false;
	status = TCP_STATUS_NOT_CONN;
	recvBufSize = sendBufSize = 0;
}

TCPSession::~TCPSession()
{
	if (listener)
		listener->destroy();

	delete tcpSocket;
}

bool TCPSession::isListen()
{
	return (status == TCP_STATUS_LISTEN);
}

void TCPSession::fillHeader(TCP_PACKET_HDR *header, uint16 cmd)
{
	header->ver = htons(TCP_VER);
	header->reserved = 0;
	header->cmd = htons(cmd);
}

void TCPSession::enableWrite(bool enable)
{
	if (status != TCP_STATUS_ESTABLISHED)
		return;

	writeEnabled = enable;

	if (enable)
		tcpSocket->addEvent(Socket::WRITE);
	else if (sendBufSize == 0)
		tcpSocket->removeEvent(Socket::WRITE);
}

void TCPSession::destroy()
{
	icqMain->removeSession(this);
	delete this;
}

bool TCPSession::sendPacket(uint16 cmd, const char *data, int n)
{
	int bytes = n + 2 + sizeof(TCP_PACKET_HDR);
	if (sendBufSize + bytes > sizeof(sendBuf)) {
		ICQ_LOG("Send buffer overflow!\n");
		return false;
	}

	char *p = sendBuf + sendBufSize;
	*(uint16 *) p = htons(n + sizeof(TCP_PACKET_HDR));
	TCP_PACKET_HDR *header = (TCP_PACKET_HDR *) (p + 2);
	fillHeader(header, cmd);
	if (n > 0)
		memcpy(header + 1, data, n);

	sendBufSize += bytes;

	tcpSocket->addEvent(Socket::WRITE);
	return true;
}

void TCPSession::sendHello()
{
	string name, domain;
	parseAccount(contactName.c_str(), name, domain);

	// Local contact
	if (domain.empty())
		parseAccount(icqMain->getUserName(), name, domain);
	else
		name = icqMain->getUserName();

	OutPacket out;
	out << name.c_str() << isSend;
	sendPacket(TCP_CMD_HELLO, out.data, out.getLength());
}

void TCPSession::connect(uint32 ip, uint16 port)
{
	status = TCP_STATUS_NOT_CONN;

	tcpSocket->addEvent(Socket::WRITE);
	tcpSocket->addEvent(Socket::EXCEPTION);

	in_addr addr;
	addr.s_addr = htonl(ip);
	const char *host = inet_ntoa(addr);
	tcpSocket->connect(host, port);

	sendHello();
}

void TCPSession::listen()
{
	::listen(tcpSocket->getFd(), 5);

	status = TCP_STATUS_LISTEN;
}

void TCPSession::recvPacket()
{
	int n;

	while (true) {
		n = tcpSocket->receive(recvBuf + recvBufSize, sizeof(recvBuf) - recvBufSize);
		if (n <= 0)
			break;

		recvBufSize += n;
		char *start = recvBuf;
		char *end = start + recvBufSize;
		uint16 len;

		while (start + sizeof(len) < end) {
			len = ntohs(*(uint16 *) start);
			if (end - start - sizeof(len) < len)
				break;

			start += sizeof(len);
			onPacketReceived(start, len);
			start += len;
		}

		recvBufSize = end - start;
		if (recvBufSize > 0)
			memmove(recvBuf, start, recvBufSize);
	}

	if (n == 0)
		onClose();
}

void TCPSession::onConnect()
{
	tcpSocket->removeEvent(Socket::WRITE);

	status = TCP_STATUS_ESTABLISHED;

	onTCPEstablished();
}

void TCPSession::onAccept()
{
	TCPSession *s = icqMain->acceptSession(sessionType.c_str(), tcpSocket);
	if (s)
		s->status = TCP_STATUS_HELLO_WAIT;
}

void TCPSession::onClose()
{
	tcpSocket->close();

	if (listener)
		listener->onClose();
}

void TCPSession::onTCPEstablished()
{
	if (!listener)
		listener = icqMain->getTCPListener(sessionType.c_str(), this);

	if (!listener)
		onClose();
}

void TCPSession::onPacketReceived(const char *data, int n)
{
	if (n < sizeof(TCP_PACKET_HDR))
		return;

	TCP_PACKET_HDR header;

	InPacket in(data, n);
	in >> header.ver >> header.reserved >> header.cmd;

	if (header.cmd == TCP_CMD_HELLO)
		onHello(in);
	else if (status == TCP_STATUS_ESTABLISHED) {
		if (listener)
			listener->onReceive(header.cmd, in.cursor, in.getBytesLeft());
	}
}

void TCPSession::onHello(InPacket &in)
{
	if (status != TCP_STATUS_HELLO_WAIT)
		return;

	status = TCP_STATUS_ESTABLISHED;

	const char *name;
	in >> name >> isSend;
	isSend = !isSend;

	contactName = name;

	onTCPEstablished();
}

void TCPSession::onSocketRead()
{
	if (status == TCP_STATUS_LISTEN)
		onAccept();
	else
		recvPacket();
}

void TCPSession::onSocketWrite()
{
	if (status == TCP_STATUS_NOT_CONN)
		onConnect();

	if (sendBufSize > 0) {
		int n = tcpSocket->send(sendBuf, sendBufSize);
		if (n > 0) {
			sendBufSize -= n;
			if (sendBufSize > 0)
				memmove(sendBuf, sendBuf + n, sendBufSize);
		}
	}

	if (sendBufSize == 0) {
		if (writeEnabled)
			listener->onSend();
		else
			tcpSocket->removeEvent(Socket::WRITE);
	}
}

void TCPSession::onSocketException()
{
	ICQ_LOG("Can not connect to contact %s.\n", contactName.c_str());

	isDisabled = true;
}
