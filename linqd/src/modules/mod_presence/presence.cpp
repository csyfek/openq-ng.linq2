#include "presence.h"
#include "session.h"
#include "packet.h"
#include "dbconn.h"
#include "sqlstmt.h"
#include "proto.h"
#include "debug.h"


#define SUB_TO		0x01
#define SUB_FROM	0x02


class PRES {
public:
	PRES(Pool *p) : notifySet(p) {}

	uint32 status;
	uint16 p2pVer;
	uint32 realIP;
	uint16 msgPort;

	StringSet notifySet;

	DECLARE_POOL
};


static int moduleIndex = -1;


inline void broadcastPacket(EVENT *ev, PRES *pres, PACKET *p)
{
	for (int i = 0; i < HASH_SIZE; i++) {
		HASH_ITEM *item = pres->notifySet.buckets[i];
		while (item) {
			p->to = STR(item);
			ev->mapi->deliver(p);
			item = item->next;
		}
	}
}

inline void createOnlinePacket(OutPacket &out, Session *s, PRES *pres)
{
	out << pres->status << pres->p2pVer;
	out.write32(s->getIP());
	out.write32(pres->realIP);
	out.write16(pres->msgPort);
}

static void onlineNotify(EVENT *ev, PRES *pres, const char *c)
{
	OutPacket out;
	createOnlinePacket(out, ev->s, pres);

	PACKET p;
	p.cmd = CMD_SRV_USER_ONLINE;
	p.data = out.data;
	p.dataLen = out.getLength();
	p.from = ev->s->getUserName();
	p.to = c;
	p.online = true;

	ev->mapi->deliver(&p);
}

static void onlineNotify(EVENT *ev, PRES *pres)
{
	OutPacket out;
	createOnlinePacket(out, ev->s, pres);

	PACKET p;
	p.cmd = CMD_SRV_USER_ONLINE;
	p.data = out.data;
	p.dataLen = out.getLength();
	p.from = ev->s->getUserName();
	p.to = NULL;
	p.online = true;

	broadcastPacket(ev, pres, &p);
}

static void offlineNotify(EVENT *ev, PRES *pres)
{
	PACKET p;
	p.cmd = CMD_SRV_USER_OFFLINE;
	p.from = ev->s->getUserName();
	p.to = NULL;
	p.data = NULL;
	p.dataLen = 0;
	p.online = true;

	broadcastPacket(ev, pres, &p);
}

static void statusNotify(EVENT *ev, PRES *pres)
{
	OutPacket out;
	out << pres->status;

	PACKET p;
	p.cmd = CMD_SRV_USER_STATUS;
	p.data = out.data;
	p.dataLen = out.getLength();
	p.from = ev->s->getUserName();
	p.to = NULL;

	broadcastPacket(ev, pres, &p);
}

inline bool addAndNotify(EVENT *ev, PRES *pres, const char *c)
{
	if (!pres->notifySet.add(c))
		return false;

	onlineNotify(ev, pres, c);
	return true;
}

static bool onSessionEvent(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	Pool *pool = ev->s->getPool();
	PRES *pres = new(pool) PRES(pool);

	in >> pres->status >> pres->p2pVer;
	pres->realIP = in.read32();
	pres->msgPort = in.read16();

	ev->s->registerEvent(moduleIndex, pres);

	DBResult *res;
	DB_ROW row;

	SQLStmt sql(ev->conn);
	SQL_APPEND(sql, "SELECT contact, subscrpt FROM roster WHERE id=");
	sql << ev->s->getUserID();
	if (!ev->conn->query(sql.getData(), sql.getLength()) ||
		!(res = ev->conn->getResult()))
		return false;

	const char *name = ev->s->getUserName();

	while ((row = res->fetchRow()) != NULL) {
		const char *c = row[0];
		uint8 sub = atoi(row[1]);

		if (sub & SUB_TO) {
			PACKET p;
			p.cmd = CMD_SRV_PROBE_STATUS;
			p.from = name;
			p.to = c;
			p.data = NULL;
			p.dataLen = 0;
			p.online = true;

			ev->mapi->deliver(&p);
		}

		if ((sub & SUB_FROM) && (pres->status != STATUS_INVIS))
			addAndNotify(ev, pres, c);
	}
	res->destroy();

	return false;
}

static bool onUserStatus(EVENT *ev)
{
	if (!ev->s)
		return false;

	OutPacket out;
	out << ev->packet->from;
	out.appendData(ev->packet->data, ev->packet->dataLen);

	ev->s->deliver(ev->packet->cmd, out.data, out.getLength());
	return true;
}

static bool onProbeStatus(EVENT *ev, PRES *pres)
{
	if (!ev->s)
		return false;
	
	if (!pres->notifySet.contains(ev->packet->from)) {
		ICQ_LOG("%s query %s's status, but is not qualified\n",
			ev->packet->from, ev->packet->to);
		return true;
	}

	if (pres->status == STATUS_INVIS) {
		ICQ_LOG("%s query %s's status, but i am invisible now!\n",
			ev->packet->from, ev->packet->to);
		return true;
	}

	onlineNotify(ev, pres, ev->packet->from);
	return true;
}

static bool onChangeStatus(EVENT *ev, PRES *pres)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint32 status;
	in >> status;
	if (status == STATUS_OFFLINE || pres->status == status)
		return true;

	uint32 oldStatus = pres->status;
	pres->status = status;

	if (oldStatus == STATUS_INVIS)
		onlineNotify(ev, pres);
	else if (status == STATUS_INVIS)
		offlineNotify(ev, pres);
	else
		statusNotify(ev, pres);

	return true;
}

static bool onContactAuthOut(EVENT *ev, PRES *pres)
{
	if (!ev->s)
		return false;
	
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint8 auth;
	in >> auth;

	if (auth == AUTH_ACCEPT && pres->status != STATUS_INVIS)
		addAndNotify(ev, pres, ev->packet->to);

	return false;
}

static bool onMessageOut(EVENT *ev, PRES *pres)
{
	if (!ev->s)
		return false;
	
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint32 when;
	uint8 type;
	in >> when >> type;

	if (type == MSG_AUTH_ACCEPT)
		addAndNotify(ev, pres, ev->packet->to);

	return false;
}

static bool onSessionEnd(EVENT *ev, PRES *pres)
{
	if (pres->status != STATUS_INVIS)
		offlineNotify(ev, pres);

	return false;
}

static bool onSessionOut(EVENT *ev, PRES *pres)
{
	switch (ev->packet->cmd) {
	case CMD_CHANGE_STATUS:
		return onChangeStatus(ev, pres);

	case CMD_SRV_CONTACT_AUTH:
		return onContactAuthOut(ev, pres);

	case CMD_SRV_MESSAGE:
		return onMessageOut(ev, pres);
	}
	return false;
}

static bool onSessionIn(EVENT *ev, PRES *pres)
{
	switch (ev->packet->cmd) {
	case CMD_SRV_PROBE_STATUS:
		return onProbeStatus(ev, pres);

	case CMD_SRV_USER_ONLINE:
	case CMD_SRV_USER_OFFLINE:
	case CMD_SRV_USER_STATUS:
		return onUserStatus(ev);

	case CMD_SRV_DEL_CONTACT:
		pres->notifySet.remove(ev->packet->from);
		break;
	}
	return false;
}


bool Presence::handleEvent(EVENT *ev, void *data)
{
	PRES *pres = (PRES *) data;

	switch (ev->event) {
	case EV_SESSION:
		return onSessionEvent(ev);

	case EV_S_OUT:
		return onSessionOut(ev, pres);

	case EV_S_IN:
		return onSessionIn(ev, pres);

	case EV_S_END:
		return onSessionEnd(ev, pres);
	}
	return false;
}

bool Presence::init(int module, MAPI *mapi, Profile *prof)
{
	moduleIndex = module;

	mapi->registerEvent(module, EV_SESSION);
	return true;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new Presence();
}
