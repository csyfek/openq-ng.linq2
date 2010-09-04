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

#ifndef _ICQ_MAIN_H
#define _ICQ_MAIN_H

#pragma warning(disable:4786)

#include "udpsession.h"
#include "tcpsession.h"
#include "proxy.h"
#include <list>
#include <map>

using namespace std;


class SocketRegistry;

/*
 * The main central class
 */
class ICQMain : public SessionListener {
public:
	ICQMain(SocketRegistry *reg);
	virtual ~ICQMain();

	// Get a TcpSessionListener according to session type
	virtual TCPSessionListener *getTCPListener(const char *type, TCPSessionBase *session);

	virtual void onRecvMessage(ICQ_MSG &msg);
	virtual void onUserOnline(ONLINE_INFO &info);
	virtual void onUserOffline(const char *name);

	const char *getUserName() { return userName.c_str(); }
	UDPSession *getUDPSession() { return udpSession; }

	void setProxy(PROXY_INFO *proxy);

	// Connect to the given host. This function is asynchronous.
	// When connection is established, SessionListener::onConnect() is called
	void connect(const char *host, uint16 port);

	// Accept a new session upon a tcp connection request
	TCPSession *acceptSession(const char *type, Socket *sock);

	// Remove a session from session list
	void removeSession(TCPSession *s);

	void login(const char *name, const char *passwd, uint32 status);
	void logout();
	uint32 sendMessage(const char *to, const char *text);
	void sendTCPRequest(const char *type, const char *to, const char *nick, const char *reason);
	void acceptTCPRequest(const char *type, const char *to, uint16 port);

private:
	ONLINE_INFO *getOnlineInfo(const char *name);
	TCPSession *findSession(const char *type, const char *name);
	uint16 createListenSession(const char *type);
	TCPSession *createTCPSession(const char *type, ONLINE_INFO *c, uint16 port, bool isSend);
	void destroyAllSessions();

	string userName;
	UDPSession *udpSession;
	PROXY_INFO *proxyInfo;

	// Online information of all contacts
	map<string, ONLINE_INFO> onlineInfoMap;

	// TCPSession list
	list<TCPSession *> tcpSessionList;
};


#endif
