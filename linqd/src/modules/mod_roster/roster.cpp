#include "roster.h"
#include "sqlstmt.h"
#include "session.h"
#include "packet.h"
#include "proto.h"
#include "debug.h"


#define SUB_TO		0x01
#define SUB_FROM	0x02


static uint32 getUserID(DBConn *conn, const char *name)
{
	DBResult *res;

	SQLStmt sql(conn);
	SQL_APPEND(sql, "SELECT id FROM user_tbl WHERE name=");
	sql << name;
	if (!conn->query(sql.getData(), sql.getLength()) ||
		!(res = conn->getResult()))
		return 0;

	DB_ROW row = res->fetchRow();
	uint32 id = (row ? atol(row[0]) : 0);
	res->destroy();

	return id;
}

inline uint32 getUserID(DBConn *conn, Session *s, const char *name)
{
	return (s ? s->getUserID() : getUserID(conn, name));
}

static bool dbAddContact(DBConn *conn, uint32 id, const char *c, uint8 sub)
{
	DBResult *res;

	SQLStmt sql(conn);
	SQL_APPEND(sql, "SELECT subscrpt FROM roster WHERE id=");
	sql << id;
	SQL_APPEND(sql, " AND contact=");
	sql << c;
	if (!conn->query(sql.getData(), sql.getLength()) ||
		!(res = conn->getResult()))
		return false;

	DB_ROW row = res->fetchRow();
	uint8 subscrpt = (row ? atoi(row[0]) : 0);
	res->destroy();

	if (subscrpt & sub)
		return false;

	sql.reset();

	if (!subscrpt) {
		SQL_APPEND(sql, "INSERT INTO roster VALUES(");
		sql << id << ',' << c << ',' << (uint32) sub << ')';
	} else {
		subscrpt |= sub;

		SQL_APPEND(sql, "UPDATE roster SET subscrpt=");
		sql << (uint32) subscrpt;
		SQL_APPEND(sql, " WHERE id=");
		sql << id;
		SQL_APPEND(sql, " AND contact=");
		sql << c;
	}

	return conn->query(sql.getData(), sql.getLength());
}

static bool dbDelContact(DBConn *conn, uint32 id, const char *c, uint8 sub)
{
	DBResult *res;

	SQLStmt sql(conn);
	SQL_APPEND(sql, "SELECT subscrpt FROM roster WHERE id=");
	sql << id;
	SQL_APPEND(sql, " AND contact=");
	sql << c;
	if (!conn->query(sql.getData(), sql.getLength()) ||
		!(res = conn->getResult()))
		return false;

	DB_ROW row = res->fetchRow();
	uint8 subscrpt = (row ? atoi(row[0]) : 0);
	res->destroy();

	if (!subscrpt)
		return false;
	
	subscrpt &= ~sub;
	sql.reset();

	if (!subscrpt) {
		SQL_APPEND(sql, "DELETE FROM roster WHERE id=");
		sql << id;
		SQL_APPEND(sql, " AND contact=");
		sql << c;
	} else {
		SQL_APPEND(sql, "UPDATE roster SET subscrpt=");
		sql << (uint32) subscrpt;
		SQL_APPEND(sql, " WHERE id=");
		sql << id;
		SQL_APPEND(sql, " AND contact=");
		sql << c;
	}

	return conn->query(sql.getData(), sql.getLength());
}

bool Roster::init(int module, MAPI *mapi, Profile *prof)
{
	mapi->registerEvent(module, EV_S_OUT);
	mapi->registerEvent(module, EV_S_IN);
	return true;
}

static bool onAddContact(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	ICQ_STR to;
	in >> to;

	PACKET p;
	p.cmd = CMD_SRV_ADD_CONTACT;
	p.from = ev->s->getUserName();
	p.to = to.text;
	p.data = NULL;
	p.dataLen = 0;
	p.online = false;

	ev->mapi->deliver(&p);
	return true;
}

static bool onDelContact(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	ICQ_STR to;
	in >> to;

	if (!dbDelContact(ev->conn, ev->s->getUserID(), to.text, SUB_TO))
		return true;

	PACKET p;
	p.cmd = CMD_SRV_DEL_CONTACT;
	p.from = ev->s->getUserName();
	p.to = to.text;
	p.data = NULL;
	p.dataLen = 0;
	p.online = false;

	ev->mapi->deliver(&p);
	return true;
}

static bool onGetContactList(EVENT *ev)
{
	DBResult *res;
	DB_ROW row;

	SQLStmt sql(ev->conn);
	SQL_APPEND(sql, "SELECT contact FROM roster WHERE id=");
	sql << ev->s->getUserID();
	SQL_APPEND(sql, " AND subscrpt&");
	sql << (uint32) SUB_TO;

	if (!ev->conn->query(sql.getData(), sql.getLength()) ||
		!(res = ev->conn->getResult()))
		return true;

	OutPacket out;
	uint16 n = 0;
	out.cursor += sizeof(n);

	while ((row = res->fetchRow()) != NULL) {
		unsigned long *lengths = res->fetchLengths();
		ICQ_STR c = { lengths[0], row[0] };

		if (out.getBytesLeft() < c.len + 2)
			break;

		out << c;
		n++;
	}
	res->destroy();

	*(uint16 *) out.data = htons(n);
	ev->s->deliver(CMD_GET_CONTACT_LIST, out.data, out.getLength());
	return true;
}

static bool onContactAuthIn(EVENT *ev)
{
	if (!ev->s)
		return false;

	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint8 auth;
	in >> auth;

	if (auth == AUTH_ACCEPT)
		dbAddContact(ev->conn, ev->s->getUserID(), ev->packet->from, SUB_TO);

	OutPacket out;
	out << ev->packet->from;
	out << auth;
	ev->s->deliver(CMD_ADD_CONTACT, out.data, out.getLength());

	return true;
}

static bool onContactAuthOut(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint8 auth;
	in >> auth;

	if (auth != AUTH_ACCEPT)
		return false;

	uint32 id = getUserID(ev->conn, ev->s, ev->packet->from);
	if (id)
		dbAddContact(ev->conn, id, ev->packet->to, SUB_FROM);

	return false;
}

static bool onMessageOut(EVENT *ev)
{
	if (!ev->s)
		return false;

	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint32 when;
	uint8 type;
	in >> when >> type;

	if (type == MSG_AUTH_ACCEPT)
		dbAddContact(ev->conn, ev->s->getUserID(), ev->packet->to, SUB_FROM);

	return false;
}

static bool onMessageIn(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint32 when;
	uint8 type;
	in >> when >> type;

	if (type != MSG_AUTH_ACCEPT)
		return false;

	uint32 id = getUserID(ev->conn, ev->s, ev->packet->to);

	dbAddContact(ev->conn, id, ev->packet->from, SUB_TO);
	return false;
}

static bool onDelContactIn(EVENT *ev)
{
	uint32 id = getUserID(ev->conn, ev->s, ev->packet->to);
	dbDelContact(ev->conn, id, ev->packet->from, SUB_FROM);
	return true;
}

static bool onSessionOut(EVENT *ev)
{
	switch (ev->packet->cmd) {
	case CMD_ADD_CONTACT:
		return onAddContact(ev);

	case CMD_DEL_CONTACT:
		return onDelContact(ev);

	case CMD_GET_CONTACT_LIST:
		return onGetContactList(ev);

	case CMD_SRV_CONTACT_AUTH:
		return onContactAuthOut(ev);

	case CMD_SRV_MESSAGE:
		return onMessageOut(ev);
	}
	return false;
}

static bool onSessionIn(EVENT *ev)
{
	switch (ev->packet->cmd) {
	case CMD_SRV_CONTACT_AUTH:
		return onContactAuthIn(ev);

	case CMD_SRV_DEL_CONTACT:
		return onDelContactIn(ev);

	case CMD_SRV_MESSAGE:
		return onMessageIn(ev);
	}
	return false;
}

bool Roster::handleEvent(EVENT *ev, void *data)
{
	switch (ev->event) {
	case EV_S_OUT:
		return onSessionOut(ev);

	case EV_S_IN:
		return onSessionIn(ev);
	}
	return false;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new Roster;
}
