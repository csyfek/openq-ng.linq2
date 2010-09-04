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

#include "icqmain.h"
#include "tcpsession.h"
#include "messagesession.h"
#include "textstream.h"
#include "nullproxy.h"
#include "socksudpproxy.h"
#include "sockstcpproxy.h"
#include "httpproxy.h"
#include "icqlog.h"
#include "icqutils.h"
#include <assert.h>


#define MESSAGE_SESSION		"MESSAGE"


SocketRegistry *socketRegistry;


ICQMain::ICQMain(SocketRegistry *reg)
{
#ifdef _WIN32
	WSADATA data;
	WSAStartup(MAKEWORD(2, 2), &data);
#endif

	socketRegistry = reg;

	proxyInfo = NULL;
	udpSession = new UDPSession(this);

#ifndef _DEBUG
	srand(time(NULL));
#endif
}

ICQMain::~ICQMain()
{
	if (proxyInfo)
		delete proxyInfo;

	destroyAllSessions();
	delete udpSession;

#ifdef _WIN32
	WSACleanup();
#endif
}

TCPSessionListener *ICQMain::getTCPListener(const char *type, TCPSessionBase *session)
{
	// The core only knows message session
	if (strcmp(type, MESSAGE_SESSION) == 0)
		return new MessageSession(session, this);
	return NULL;
}

ONLINE_INFO *ICQMain::getOnlineInfo(const char *name)
{
	map<string, ONLINE_INFO>::iterator it = onlineInfoMap.find(name);
	if (it == onlineInfoMap.end())
		return NULL;

	return &(*it).second;
}

// Set proxy information. NULL means no proxy
void ICQMain::setProxy(PROXY_INFO *proxy)
{
	if (proxyInfo) {
		delete proxyInfo;
		proxyInfo = NULL;
	}

	if (proxy) {
		proxyInfo = new PROXY_INFO;
		*proxyInfo = *proxy;
	}
}

void ICQMain::connect(const char *host, uint16 port)
{
	Socket *sock;

	// Use various XXXProxy according to proxy type
	if (!proxyInfo)
		sock = new NullProxy;
	else if (proxyInfo->type == PROXY_SOCKS5)
		sock = new SocksUDPProxy(*proxyInfo);
	else
		sock = new HTTPProxy(*proxyInfo);

	udpSession->connect(sock, host, port);
}

// Find a session according to type and name
TCPSession *ICQMain::findSession(const char *type, const char *name)
{
	list<TCPSession *>::iterator it;

	for (it = tcpSessionList.begin(); it != tcpSessionList.end(); ++it) {
		TCPSession *s = *it;
		if (s->sessionType == type && s->contactName == name)
			return s;
	}
	return NULL;
}

/*
 * Get/Create a listen session
 * Returns the port that this session binds on
 */
uint16 ICQMain::createListenSession(const char *type)
{
	TCPSession *session = NULL;

	// Search whether we already have one
	list<TCPSession *>::iterator it;
	for (it = tcpSessionList.begin(); it != tcpSessionList.end(); ++it) {
		TCPSession *s = *it;
		if (s->isListen() && s->sessionType == type) {
			session = s;
			break;
		}
	}

	int fd;
	sockaddr_in addr;

	// No session found, create a socket and listens on some port
	if (!session) {
		fd = socket(AF_INET, SOCK_STREAM, 0);
		assert(fd >= 0);

		memset(&addr, 0, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = INADDR_ANY;
		addr.sin_port = 0;	// Let OS assign a port for us
		bind(fd, (sockaddr *) &addr, sizeof(addr));

		Socket *sock = new NullProxy(fd);
		session = new TCPSession(this, sock, type, "", false);

		session->listen();
		tcpSessionList.push_back(session);
	}

	fd = session->getSocketFd();

	// Get the port we are listening on
	socklen_t len = sizeof(addr);
	getsockname(fd, (sockaddr *) &addr, &len);

	return ntohs(addr.sin_port);
}

TCPSession *ICQMain::createTCPSession(const char *type, ONLINE_INFO *c, uint16 port, bool isSend)
{
	Socket *sock;
	if (proxyInfo && proxyInfo->type == PROXY_SOCKS5)
		sock = new SocksTCPProxy(*proxyInfo);
	else
		sock = new NullProxy;

	TCPSession *session = new TCPSession(this, sock, type, c->name.c_str(), isSend);
	session->connect(c->realip, port);

	// Do not forget to put it into the list
	tcpSessionList.push_back(session);
	return session;
}

TCPSession *ICQMain::acceptSession(const char *type, Socket *sock)
{
	int fd = accept(sock->getFd(), NULL, NULL);
	if (fd < 0)
		return NULL;

	sock = new NullProxy(fd);
	TCPSession *session = new TCPSession(this, sock, type, "", false);
	tcpSessionList.push_back(session);

	return session;
}

void ICQMain::removeSession(TCPSession *s)
{
	tcpSessionList.remove(s);
}

void ICQMain::destroyAllSessions()
{
	list<TCPSession *>::iterator it;
	for (it = tcpSessionList.begin(); it != tcpSessionList.end(); ++it)
		delete *it;

	tcpSessionList.clear();
}

void ICQMain::login(const char *user, const char *passwd, uint32 status)
{
	userName = user;

	string name, domain;
	parseAccount(user, name, domain);

	onlineInfoMap.clear();

	// Listens on p2p messages from others
	uint16 port = createListenSession(MESSAGE_SESSION);
	udpSession->login(name.c_str(), passwd, status, port);
}

void ICQMain::logout()
{
	udpSession->logout();

	onlineInfoMap.clear();
	destroyAllSessions();
}

/**
 * Send p2p message
 * @return 0 if send message failed
 */
uint32 ICQMain::sendMessage(const char *to, const char *text)
{
	TCPSession *session = findSession(MESSAGE_SESSION, to);
	if (!session) {
		ONLINE_INFO *c = getOnlineInfo(to);
		if (!c)
			return 0;

		// Can not send message if destination is behind a firewall
		if (!udpSession->isBehindWall() && (c->ip != c->realip))
			return 0;

		session = createTCPSession(MESSAGE_SESSION, c, c->msgport, true);
		session->setListener(new MessageSession(session, this));
	}

	// The destination is not connectabe
	if (!session->isConnectable())
		return 0;

	// HACK: Since this is a message session,
	// we are sure the listener is of type MessageSession
	assert(session->listener != NULL);

	MessageSession *s = (MessageSession *) session->listener;
	return s->sendMessage(text);
}

// Send a p2p request
void ICQMain::sendTCPRequest(const char *type, const char *to, const char *nick, const char *reason)
{
	ONLINE_INFO *c = getOnlineInfo(to);
	if (!c)
		return;

	uint16 port = 0;

	// We need reverse connection
	if (!udpSession->isBehindWall() && c->ip != c->realip)
		port = createListenSession(type);

	TextOutStream text;
	text << type << port << nick << reason;
	udpSession->sendMessage(MSG_TCP_REQUEST, to, text);
}

// Accept a tcp request
void ICQMain::acceptTCPRequest(const char *type, const char *to, uint16 port)
{
	// If port is non-zero, this is a reverse tcp connection
	if (port) {
		ONLINE_INFO *c = getOnlineInfo(to);
		if (c)
			createTCPSession(type, c, port, false);

	} else {
		port = createListenSession(type);

		TextOutStream out;
		out << type << port;
		udpSession->sendMessage(MSG_TCP_ACCEPT, to, out);
	}
}

void ICQMain::onRecvMessage(ICQ_MSG &msg)
{
	if (msg.type != MSG_TCP_ACCEPT)
		return;

	TextInStream in(msg.text.c_str());

	string name;
	uint16 port;
	in >> name >> port;

	// Connect to remote
	if (port) {
		ONLINE_INFO *c = getOnlineInfo(msg.contact.c_str());
		if (c)
			createTCPSession(name.c_str(), c, port, true);
	}
}

void ICQMain::onUserOnline(ONLINE_INFO &info)
{
	onlineInfoMap[info.name] = info;
}

void ICQMain::onUserOffline(const char *name)
{
	onlineInfoMap.erase(name);
}
