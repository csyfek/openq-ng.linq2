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

#ifndef _UDP_SESSION_H
#define _UDP_SESSION_H

#include "udppacket.h"
#include "socket.h"
#include "sessionlistener.h"
#include <list>

using namespace std;

// Maximum attempts before a packet is dropped
#define MAX_SEND_ATTEMPTS	3

#define SEND_TIMEOUT		6
#define KEEPALIVE_TIMEOUT	120


class UDPSession : public SocketListener {
public:
	UDPSession(SessionListener *l);
	virtual ~UDPSession();

	// SocketListener...
	virtual void onSocketRead();
	virtual void onSocketWrite();
	virtual void onSocketException();

	// Get the last packet sequence being sent recently
	uint16 getLastSeq() { return sendSeq; }
	// Whether we are behind a firewall
	bool isBehindWall() { return realIP != ourIP; }

	void connect(Socket *sock, const char *host, int port);
	void checkSendQueue();
	void sendKeepAlive();
	void registerUser(const char *name, const char *passwd);
	void login(const char *name, const char *passwd, uint32 status, uint16 port);
	void logout();
	void changeStatus(uint32 status);
	void sendMessage(uint8 type, const char *to, const char *text);
	void sendAnnounce(const char *text);
	void searchRandom();
	void searchUser(const char *name, const char *nick, const char *email);
	void addContact(const char *name);
	void delContact(const char *name);
	void getContactList();
	void getContactInfo(const char *name);
	void getUserInfo();
	void updateUserInfo(USER_INFO &info);
	void changePasswd(const char *passwd);

	void onPacketReceived(UDPInPacket &in);

	// Number of users currently online
	uint32 numClients;

private:
	void reset();
	void clearSendQueue();
	bool checkSeq(uint16 seq);

	void createPacket(UDPOutPacket &out, uint16 cmd, uint16 seq);
	UDPOutPacket *createPacket(uint16 cmd);
	void sendAck(uint16 seq);
	void sendPacket(UDPOutPacket *out);
	void sendDirect(OutPacket *out);

	void onAck(UDPInPacket &in);
	void onRegisterReply(UDPInPacket &in);
	void onLoginReply(UDPInPacket &in);
	void onAddContactReply(UDPInPacket &in);
	void onContactListReply(UDPInPacket &in);

	void onUserOnline(UDPInPacket &in);
	void onUserOffline(UDPInPacket &in);
	void onUserStatus(UDPInPacket &in);
	void onSearchResult(UDPInPacket &in);
	void onRecvMessage(UDPInPacket &in);
	void onContactInfoReply(UDPInPacket &in);
	void onUserInfoReply(UDPInPacket &in);

	SessionListener *listener;
	Socket *udpSocket;

	uint32 sessionID;
	uint32 realIP;	// inner ip
	uint32 ourIP;	// outer ip
	uint16 sendSeq, recvSeq;
	uint8 seqWindow[1 << 13];
	list<UDPOutPacket *> sendQueue;
};


#endif
