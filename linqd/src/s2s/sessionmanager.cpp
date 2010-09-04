#include "sessionmanager.h"
#include "s2s.h"
#include "proto.h"
#include "debug.h"
#include <ctype.h>


SessionManager::SessionManager(int fd) : TCPStream(fd)
{
}

void SessionManager::deliver(PACKET *p)
{
	OutPacket out;
	out << p->to << p->from << p->online << p->cmd;

	sendPacket(out, p->data, p->dataLen);
}

void SessionManager::onSocketClose()
{
	s2s.removeClient(this);
}

void SessionManager::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);

	ICQ_STR to, from;
	uint8 online;
	uint16 cmd;
	in >> to >> from >> online >> cmd;

	char *p = strchr(to.text, '@');
	if (!p) {
		ICQ_LOG("Invalid user %s\n", to.text);
		return;
	}
	*p++ = '\0';

	const char *host = p;
	for (; *p; p++)
		*p = tolower(*p);

	Server *s = s2s.getServer(host);
	if (!s) {
		s = new Server(host);
		s2s.serverList.add(&s->listItem);
		s2s.serverHash.put(s);
	}

	PACKET packet;
	packet.to = to.text;
	packet.from = from.text;
	packet.online = online;
	packet.cmd = cmd;
	packet.data = in.cursor;
	packet.dataLen = in.getBytesLeft();

	s->deliver(&packet);
}
