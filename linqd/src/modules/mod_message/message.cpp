#include "message.h"
#include "session.h"
#include "proto.h"
#include "packet.h"
#include "sqlstmt.h"


static ICQ_STR emptyStr = { 0, "" };


inline void sendMessage(Session *s, const char *from, uint32 when, uint8 type, ICQ_STR &text)
{
	OutPacket out;
	out << from << when << type << text;
	s->deliver(CMD_SRV_MESSAGE, out.data, out.getLength());
}

static bool onMessage(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint8 type;
	ICQ_STR to, text;
	in >> to >> type >> text;

	OutPacket out;
	out << (uint32) ev->mapi->getTime() << type << text;

	PACKET p;
	p.cmd = CMD_SRV_MESSAGE;
	p.from = ev->s->getUserName();
	p.to = to.text;
	p.data = out.data;
	p.dataLen = out.getLength();
	p.online = false;

	ev->mapi->deliverFrom(&p);
	return true;
}

static bool onContactAuthOut(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint8 auth;
	in >> auth;

	if (auth != AUTH_ACCEPT)
		return false;

	uint8 type = MSG_ADDED;
	uint32 when = ev->mapi->getTime();
	const char *from = ev->packet->to;

	if (ev->s)
		sendMessage(ev->s, from, when, type, emptyStr);
	else {
		// Let other module to handle it
		OutPacket out;
		out << when << type << emptyStr;

		PACKET p;
		p.cmd = CMD_SRV_MESSAGE;
		p.from = from;
		p.to = ev->packet->from;
		p.online = false;
		p.data = out.data;
		p.dataLen = out.getLength();
		ev->mapi->deliver(&p);
	}

	return false;
}

static bool onSessionOut(EVENT *ev)
{
	switch (ev->packet->cmd) {
	case CMD_MESSAGE:
		return onMessage(ev);

	case CMD_SRV_CONTACT_AUTH:
		return onContactAuthOut(ev);
	}
	return false;
}

static bool onMessageIn(EVENT *ev)
{
	if (!ev->s)
		return false;

	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint32 when;
	uint8 type;
	ICQ_STR text;
	in >> when >> type >> text;

	sendMessage(ev->s, ev->packet->from, when, type, text);
	return true;
}

bool Message::handleEvent(EVENT *ev, void *data)
{
	switch (ev->event) {
	case EV_S_IN:
		if (ev->packet->cmd == CMD_SRV_MESSAGE)
			return onMessageIn(ev);
		break;

	case EV_S_OUT:
		return onSessionOut(ev);
	}
	return false;
}

bool Message::init(int module, MAPI *mapi, Profile *prof)
{
	mapi->registerEvent(module, EV_S_OUT);
	mapi->registerEvent(module, EV_S_IN);
	return true;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new Message;
}
