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

#include "httpproxy.h"
#include "nullproxy.h"
#include "base64.h"
#include "icqlog.h"


static bool resolveHost(string &host)
{
	in_addr addr;
	addr.s_addr = inet_addr(host.c_str());

	if (addr.s_addr == INADDR_NONE) {
		hostent *hent = gethostbyname(host.c_str());
		if (!hent)
			return false;

		addr = *(in_addr *) hent->h_addr;
	}

	host = inet_ntoa(addr);
	return true;
}


enum {
	HTTP_STATUS_NOT_CONN,
	HTTP_STATUS_ESTABLISHED
};


HTTPProxy::HTTPProxy(PROXY_INFO &info)
{
	proxyInfo = info;

	listener = NULL;
	eventMask = 0;
	status = HTTP_STATUS_NOT_CONN;
	bufSize = 0;

	tcpSocket = new NullProxy;
}

HTTPProxy::~HTTPProxy()
{
	delete tcpSocket;
}

void HTTPProxy::fireEvent(int event)
{
	if (((1 << event) & eventMask) == 0)
		return;

	if (event == READ)
		listener->onSocketRead();
	else if (event == WRITE)
		listener->onSocketWrite();
	else if (event == EXCEPTION) {
		close();
		listener->onSocketException();
	}
}

void HTTPProxy::addEvent(int event)
{
	eventMask |= (1 << event);

	if (status == HTTP_STATUS_ESTABLISHED)
		tcpSocket->addEvent(event);
}

void HTTPProxy::removeEvent(int event)
{
	eventMask &= ~(1 << event);

	if (status == HTTP_STATUS_ESTABLISHED)
		tcpSocket->removeEvent(event);
}

bool HTTPProxy::create(int type, SocketListener *l)
{
	listener = l;
	return tcpSocket->create(SOCK_STREAM, this);
}

void HTTPProxy::connect(const char *host, uint16 port)
{
	destHost = host;

	tcpSocket->addEvent(WRITE);
	tcpSocket->addEvent(EXCEPTION);
	tcpSocket->connect(proxyInfo.host.c_str(), proxyInfo.port);
}

int HTTPProxy::send(const char *data, int size)
{
	uint16 n = htons(size);
	if (tcpSocket->send((const char *) &n, sizeof(n)) < 0)
		return -1;

	return tcpSocket->send(data, size);
}

int HTTPProxy::receive(char *data, int n)
{
	uint16 len;

	if (bufSize <= sizeof(len))
		return -1;

	len = ntohs(*(uint16 *) buffer);

	if (n < len || bufSize < len + sizeof(len))
		return -1;

	memcpy(data, buffer + sizeof(len), len);
	return len;
}

void HTTPProxy::onSocketWrite()
{
	if (status == HTTP_STATUS_ESTABLISHED) {
		fireEvent(WRITE);
		return;
	}

	tcpSocket->removeEvent(WRITE);
	tcpSocket->addEvent(READ);

	string host = destHost;
	if (proxyInfo.resolve)
		resolveHost(host);

	// Send 'CONNECT' request
	string req = "CONNECT ";
	req += destHost + ":443";
	req += " HTTP/1.0\r\n";
	req += "User-agent: MyICQ\r\n";

	if (!proxyInfo.user.empty()) {
		// Encode using base64
		string str = proxyInfo.user + ":" + proxyInfo.passwd;
		char *code = NULL;
		encode_base64(str.c_str(), str.length(), &code);
		if (code) {
			req += "Proxy-authorization: Basic ";
			req += code;
			req += "\r\n";
			free(code);
		}
	}
	req += "\r\n";

	ICQ_LOG("http request:\n%s\n", req.c_str());

	tcpSocket->send(req.c_str(), req.length());
}

void HTTPProxy::onSocketRead()
{
	// Since we are already connected, receive real packet data
	if (status == HTTP_STATUS_ESTABLISHED) {
		recvPacket();
		return;
	}

	int n = tcpSocket->receive(buffer, sizeof(buffer));
	if (n <= 0) {
		fireEvent(EXCEPTION);
		return;
	}

	ICQ_LOG("http response:\n%.*s\n", n, buffer);

	char *seps = " ";
	char *p = strtok(buffer, seps);
	if (p)
		p = strtok(NULL, seps);
	if (!p || *p != '2') {
		fireEvent(EXCEPTION);
		return;
	}

	ICQ_LOG("http tunnel has been established.\n");

	status = HTTP_STATUS_ESTABLISHED;
	fireEvent(WRITE);
}

void HTTPProxy::onSocketException()
{
	fireEvent(EXCEPTION);
}

/*
 * Since TCP connection is really a data stream and there is no packet boundary,
 * extra things must be done. we put an extra 2 bytes(WORD) at the head of each
 * packet describing its size.
 */
void HTTPProxy::recvPacket()
{
	int n;

	while ((n = tcpSocket->receive(buffer + bufSize, sizeof(buffer) - bufSize)) > 0) {
		bufSize += n;
		uint16 len;

		while (bufSize > sizeof(len)) {
			len = ntohs(*(uint16 *) buffer);
			if (bufSize < len + sizeof(len))
				break;

			// A complete packet is received!
			fireEvent(READ);

			bufSize -= len + sizeof(len);
			if (bufSize > 0)
				memmove(buffer, buffer + len + sizeof(len), bufSize);
		}
	}

	if (n == 0)
		fireEvent(EXCEPTION);
}
