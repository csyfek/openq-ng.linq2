#include "server.h"
#include "s2s.h"
#include "debug.h"
#include <ctype.h>


enum {
	STATUS_NOT_CONN,
	STATUS_HELLO_WAIT,
	STATUS_HELLO_REPLY,
	STATUS_AUTH,
	STATUS_CONNECTED,
};

enum {
	S2S_HELLO = 1,
	S2S_HELLO_REPLY,
	S2S_DELIVER,
};


#pragma pack(1)
struct S2S_HEADER {
	uint16 ver;
	uint32 reserved;
	uint16 cmd;
};
#pragma pack()


struct PACKET_LIST {
	ListHead list;

	uint16 cmd;
	char data[1024];
	int len;
};


const uint16 S2S_VER = 1;


Server::Server(const char *name)
{
	serverName[MAX_SERVER_LEN] = '\0';
	strncpy(serverName, name, MAX_SERVER_LEN);

	status = STATUS_NOT_CONN;

	s2s.resolver->resolve(name);
}

Server::Server(int fd) : TCPStream(fd)
{
	*serverName = '\0';

	sockaddr_in addr;
	getSocketAddress(addr);
	serverIP = addr.sin_addr.s_addr;

	status = STATUS_HELLO_WAIT;
}

Server::~Server()
{
	listItem.remove();
	hashItem.remove();

	while (!packetList.isEmpty()) {
		PACKET_LIST *p = LIST_ENTRY(packetList.removeHead(), PACKET_LIST, list);
		delete p;
	}
}

void Server::sendPacket(uint16 cmd, const char *data, int len)
{
	if (cmd == S2S_DELIVER && status != STATUS_CONNECTED) {
		if (len > MAX_PACKET_SIZE)
			return;

		PACKET_LIST *p = new PACKET_LIST;
		p->cmd = cmd;
		p->len = len;
		if (len > 0)
			memcpy(p->data, data, len);
		packetList.add(&p->list);
		return;
	}

	OutPacket out;
	out << S2S_VER << (uint32) 0 << cmd;
	TCPStream::sendPacket(out, data, len);
}

void Server::deliver(PACKET *p)
{
	OutPacket out;
	out << p->to << p->from << p->online << p->cmd;
	out.appendData(p->data, p->dataLen);

	sendPacket(S2S_DELIVER, out.data, out.getLength());
}

void Server::onResolved(uint32 ip)
{
	ICQ_LOG("%s is resolved to %s\n", serverName, inet_ntoa(*(in_addr *) &ip));

	if (ip == INADDR_NONE) {
		delete this;
		return;
	}

	if (status == STATUS_NOT_CONN) {
		serverIP = ip;
		connect(ip, 8222, 60);

	} else if (status == STATUS_AUTH) {
		if (serverIP != ip) {
			ICQ_LOG("%s authentication failed\n", serverName);
			delete this;
			return;
		}

		ICQ_LOG("%s is accepted\n", serverName);

		sendPacket(S2S_HELLO_REPLY, NULL, 0);
		status = STATUS_CONNECTED;
	}
}

void Server::onSocketConnect()
{
	TCPStream::onSocketConnect();

	// Send hello packet to the remote
	OutPacket out;
	out << s2s.option.domain.c_str();
	sendPacket(S2S_HELLO, out.data, out.getLength());

	status = STATUS_HELLO_REPLY;
}

void Server::onSocketTimeout()
{
	ICQ_LOG("server timeouts\n");

	delete this;
}

void Server::onSocketClose()
{
	ICQ_LOG("Connection to %s closed\n", serverName);

	delete this;
}

void Server::onDeliver(InPacket &in)
{
	if (!s2s.sessionManager) {
		ICQ_LOG("No sm to deliver packet to\n");
		return;
	}
	if (status != STATUS_CONNECTED) {
		ICQ_LOG("Can not process packet\n");
		return;
	}

	ICQ_STR to, fromuser;
	uint8 online;
	uint16 cmd;
	in >> to >> fromuser >> online >> cmd;

	char from[1024];
	int len = strlen(serverName);
	if (fromuser.len + len + 1 >= sizeof(from))
		return;

	const char *src = fromuser.text;
	char *dst = from;
	while (*src)
		*dst++ = tolower(*src++);
	*dst++ = '@';
	memcpy(dst, serverName, len + 1);

	PACKET p;
	p.to = to.text;
	p.from = from;
	p.online = online;
	p.cmd = cmd;
	p.data = in.cursor;
	p.dataLen = in.getBytesLeft();

	s2s.sessionManager->deliver(&p);
}

void Server::onHello(InPacket &in)
{
	if (status != STATUS_HELLO_WAIT) {
		ICQ_LOG("Can not process hello packet\n");
		return;
	}

	ICQ_STR name;
	in >> name;

	if (name.len > MAX_SERVER_LEN) {
		ICQ_LOG("name is too long %d\n", name.len);
		delete this;
	}

	memcpy(serverName, name.text, name.len + 1);

	s2s.serverHash.put(this);
	s2s.resolver->resolve(serverName);

	status = STATUS_AUTH;
}

void Server::onHelloReply(InPacket &in)
{
	if (status != STATUS_HELLO_REPLY) {
		ICQ_LOG("Can not process hello-reply packet\n");
		return;
	}

	ICQ_LOG("Yeah! I am authenticated by the remote\n");

	status = STATUS_CONNECTED;

	while (!packetList.isEmpty()) {
		PACKET_LIST *p = LIST_ENTRY(packetList.removeHead(), PACKET_LIST, list);
		sendPacket(p->cmd, p->data, p->len);
		delete p;
	}
}

void Server::onPacketReceived(InPacket &in, uint16 cmd)
{
	switch (cmd) {
	case S2S_HELLO:
		onHello(in);
		break;

	case S2S_HELLO_REPLY:
		onHelloReply(in);
		break;

	case S2S_DELIVER:
		onDeliver(in);
		break;
	}
}

void Server::onPacketReceived(const char *data, int n)
{
	n -= sizeof(S2S_HEADER);
	if (n < 0)
		return;

	S2S_HEADER *header = (S2S_HEADER *) data;
	header->ver = ntohs(header->ver);
	header->reserved = ntohl(header->reserved);
	header->cmd = ntohs(header->cmd);

	InPacket in((char *) (header + 1), n);
	onPacketReceived(in, header->cmd);
}
