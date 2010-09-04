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

#ifndef _TCP_SESSION_H
#define _TCP_SESSION_H

#include "tcpsessionbase.h"
#include "socket.h"
#include "packet.h"
#include <string>
#include <list>

using namespace std;


#define SEND_BUF_SIZE		4096
#define RECV_BUF_SIZE		1024

struct TCP_PACKET_HDR;
class ICQMain;


class TCPSession : public TCPSessionBase, public SocketListener {
public:
	TCPSession(ICQMain *main, Socket *sock, const char *type, const char *name, bool isSend);
	virtual ~TCPSession();

	int getSocketFd() { return tcpSocket->getFd(); }
	bool isConnectable() { return !isDisabled; }
	void setListener(TCPSessionListener *l) { listener = l; }
	bool isListen();

	virtual const char *getContactName() { return contactName.c_str(); }
	virtual bool isSender() { return !!isSend; }
	virtual void enableWrite(bool enable);
	virtual void destroy();
	virtual bool sendPacket(uint16 cmd, const char *data, int n);

	virtual void onSocketRead();
	virtual void onSocketWrite();
	virtual void onSocketException();

	void connect(uint32 ip, uint16 port);
	void listen();


	ICQMain *icqMain;
	TCPSessionListener *listener;
	string sessionType;
	string contactName;

private:
	void fillHeader(TCP_PACKET_HDR *header, uint16 cmd);
	void recvPacket();
	void sendHello();

	void onConnect();
	void onAccept();
	void onClose();
	void onTCPEstablished();
	void onHello(InPacket &in);
	void onPacketReceived(const char *data, int n);

	Socket *tcpSocket;

	uint8 isSend;
	bool writeEnabled;
	bool isDisabled;
	int status;
	char recvBuf[RECV_BUF_SIZE];
	int recvBufSize;
	char sendBuf[SEND_BUF_SIZE];
	int sendBufSize;
};


#endif
