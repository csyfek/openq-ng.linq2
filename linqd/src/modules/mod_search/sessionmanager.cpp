#include "sessionmanager.h"
#include "proto.h"
#include "debug.h"


SessionManager::SessionManager(MAPI *m)
{
	mapi = m;
}

void SessionManager::addSession(const char *name, const char *data, int n)
{
	OutPacket out;
	out << (uint16) SM_ADD_SESSION;
	out << name;

	sendPacket(out, data, n);
}

void SessionManager::removeSession(const char *name)
{
	OutPacket out;
	out << (uint16) SM_REMOVE_SESSION;
	out << name;

	sendPacket(out, NULL, 0);
}

bool SessionManager::deliver(PACKET *p)
{
	OutPacket out;
	out << (uint16) SM_DELIVER;
	out << p->to << p->from << p->online << p->cmd;

	return sendPacket(out, p->data, p->dataLen);
}

void SessionManager::sendRequest(const char *from, PACKET *p)
{
	OutPacket out;
	out << (uint16) SM_REQUEST;
	out << from << (uint16) p->cmd;

	sendPacket(out, p->data, p->dataLen);
}

void SessionManager::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);

	ICQ_STR to, from;
	uint16 cmd;
	in >> to >> from >> cmd;

	PACKET p;
	p.to = to.text;
	p.from = from.text;
	p.cmd = cmd;
	p.data = in.cursor;
	p.dataLen = in.getBytesLeft();

	mapi->deliverLocal(&p);
}
