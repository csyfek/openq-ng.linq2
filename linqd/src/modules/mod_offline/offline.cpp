#include "offline.h"
#include "session.h"
#include "dbconn.h"
#include "sqlstmt.h"
#include "packet.h"
#include "proto.h"
#include <stdlib.h>


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

static bool onSessionEvent(EVENT *ev)
{
	DBResult *res;
	DB_ROW row;

	uint32 id = ev->s->getUserID();

	SQLStmt sql(ev->conn);
	SQL_APPEND(sql, "SELECT src, time, type, text FROM message WHERE id=");
	sql << id;
	SQL_APPEND(sql, " LIMIT 16");
	if (!ev->conn->query(sql.getData(), sql.getLength()) ||
		!(res = ev->conn->getResult()))
		return false;

	int n = 0;

	while ((row = res->fetchRow()) != NULL) {
		unsigned long *lengths = res->fetchLengths();

		const char *from = row[0];
		uint32 when = atol(row[1]);
		uint8 type = atoi(row[2]);
		ICQ_STR text = { lengths[3], row[3] };

		OutPacket out;
		out << from << when << type << text;

		ev->s->deliver(CMD_SRV_MESSAGE, out.data, out.getLength());
		n++;
	}
	res->destroy();

	if (n > 0) {
		sql.reset();
		SQL_APPEND(sql, "DELETE FROM message WHERE id=");
		sql << id;
		ev->conn->query(sql.getData(), sql.getLength());
	}

	return false;
}

static bool onSessionIn(EVENT *ev)
{
	static bool support = true;

	if (ev->s || ev->packet->cmd != CMD_SRV_MESSAGE)
		return false;

	DBConn *conn = ev->conn;
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint32 when;
	uint8 type;
	ICQ_STR text;
	in >> when >> type >> text;

	if (support) {
		SQLStmt sql(conn);
		SQL_APPEND(sql, "INSERT INTO message SELECT id,");
		sql << ev->packet->from << ',' << when;
		sql << ',' << (uint32) type << ',' << text;
		SQL_APPEND(sql, " FROM user_tbl WHERE name=");
		sql << ev->packet->to;

		if (conn->query(sql.getData(), sql.getLength()))
			return true;

		support = false;
	}

	uint32 id = getUserID(conn, ev->packet->to);
	if (id == 0)
		return false;

	SQLStmt sql(conn);
	SQL_APPEND(sql, "INSERT INTO message VALUES(");
	sql << id << ',' << ev->packet->from << ',' << when;
	sql << ',' << (uint32) type << ',' << text << ')';
	return conn->query(sql.getData(), sql.getLength());
}

bool Offline::handleEvent(EVENT *ev, void *data)
{
	switch (ev->event) {
	case EV_SESSION:
		return onSessionEvent(ev);

	case EV_S_IN:
		return onSessionIn(ev);
	}
	return false;
}

bool Offline::init(int module, MAPI *mapi, Profile *prof)
{
	mapi->registerEvent(module, EV_SESSION);
	mapi->registerEvent(module, EV_S_IN);
	return true;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new Offline;
}
