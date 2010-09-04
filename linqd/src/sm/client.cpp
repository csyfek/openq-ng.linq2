#include "client.h"
#include "sessionmanager.h"
#include "proto.h"
#include "debug.h"


Client::Client(int fd) : TCPStream(fd)
{
}

Client::~Client()
{
	while (!sessionList.isEmpty()) {
		Session *s = LIST_ENTRY(sessionList.removeHead(), Session, listItem);
		s->hashItem.remove();
		delete s;
	}
}

void Client::deliver(PACKET *p)
{
	OutPacket out;
	out << p->to << p->from << p->cmd;

	sendPacket(out, p->data, p->dataLen);
}

void Client::onAddSession(InPacket &in)
{
	ICQ_STR name;
	in >> name;

	Session *s = new Session(this, name.text);
	s->onChangeStatus(in);
	s->onUpdateUserInfo(in);

	sessionList.add(&s->listItem);
	sessionManager.addSession(s);
}

void Client::onRemoveSession(InPacket &in)
{
	ICQ_STR name;
	in >> name;

	Session *s = sessionManager.getSession(name.text);
	if (!s) {
		ICQ_LOG("No session %s to remove\n", name.text);
		return;
	}

	s->hashItem.remove();
	s->listItem.remove();
	delete s;
}

void Client::onDeliver(InPacket &in)
{
	sessionManager.deliverPacket(in);
}

void Client::onRequest(InPacket &in)
{
	ICQ_STR from;
	in >> from;

	Session *s = sessionManager.getSession(from.text);
	if (s)
		s->handleRequest(in);
}

void Client::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);

	uint16 cmd;
	in >> cmd;

	switch (cmd) {
	case SM_ADD_SESSION:
		onAddSession(in);
		break;

	case SM_REMOVE_SESSION:
		onRemoveSession(in);
		break;

	case SM_DELIVER:
		onDeliver(in);
		break;

	case SM_REQUEST:
		onRequest(in);
		break;

	default:
		ICQ_LOG("Unknown sm cmd\n");
	}
}

void Client::onSocketClose()
{
	TCPStream::onSocketClose();

	sessionManager.removeClient(this);
}
