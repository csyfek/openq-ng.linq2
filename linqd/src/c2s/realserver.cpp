#include "realserver.h"
#include "session.h"
#include "c2s.h"
#include "proto.h"
#include "debug.h"


// Packet header from a real server
// NOTE: Must be byte-aligned
#pragma pack(1)
struct IN_HEADER {
	uint32 ip;
	uint16 port;
	uint16 cmd;
};
#pragma pack()


RealServer::RealServer(int fd) : TCPStream(fd)
{
	next = NULL;
}

/*
 * +-------------------+
 * +  C2S_DEVLIVER     +
 * +-------------------+
 * +    User Name      +
 * +-------------------+
 * +       CMD         +
 * +-------------------+
 * +      DATA         +
 * +-------------------+
 */
void RealServer::deliver(Session *s, uint16 cmd, const char *data, int n)
{
	OutPacket out;
	out << (uint16) C2S_DELIVER;
	out << s->userName << cmd;

	sendPacket(out, data, n);
}


/*
 * +-------------------+
 * +  C2S_ADD_SESSION  +
 * +-------------------+
 * +    User Name      +
 * +-------------------+
 * +    User ID        +
 * +-------------------+
 * +    IP, Port       +
 * +-------------------+
 * +      Data         +
 * +-------------------+
 */
void RealServer::addSession(Session *s, const char *data, int n)
{
	OutPacket out;
	out << (uint16) C2S_ADD_SESSION;
	out << s->userName << s->userID;
	out.write32(s->sessionIP);
	out.write16(s->sessionPort);

	sendPacket(out, data, n);
}

/*
 * +----------------------+
 * +  C2S_REMOVE_SESSION  +
 * +----------------------+
 * +    User Name         +
 * +----------------------+
 */
void RealServer::removeSession(Session *s)
{
	OutPacket out;
	out << (uint16) C2S_REMOVE_SESSION;
	out << s->userName;

	sendPacket(out, NULL, 0);
}

void RealServer::onPacketReceived(const char *data, int n)
{
	// Must at least contains a header
	n -= sizeof(IN_HEADER);
	if (n < 0) {
		ICQ_LOG("invalid packet size\n");
		return;
	}

	IN_HEADER *h = (IN_HEADER *) data;
	h->cmd = ntohs(h->cmd);

	// Get a session by (ip, port)
	Session *s = c2s.getSession(h->ip, h->port);
	if (!s) {
		ICQ_LOG("no session to route\n");
		return;
	}

	// Deliver it to client
	s->deliver(h->cmd, (const char *) (h + 1), n);
}

void RealServer::onSocketClose()
{
	TCPStream::onSocketClose();

	// Remove it from the list
	c2s.removeServer(this);
}
