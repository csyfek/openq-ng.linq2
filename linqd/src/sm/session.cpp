#include "session.h"
#include "sessionmanager.h"
#include "client.h"
#include "sqlstmt.h"
#include "proto.h"
#include "debug.h"
#include <string.h>


#define MAX_SEARCH_RESULTS		15


inline int getAge(uint32 birth)
{
	if (!birth)
		return 0;

	return (sessionManager.curTime - birth) / (3600 * 24 * 365);
}


Session::Session(Client *c, const char *name)
{
	client = c;

	userName[MAX_NAME_LEN] = '\0';
	status = STATUS_ONLINE;
	strncpy(userName, name, MAX_NAME_LEN);
}

void Session::onUpdateUserInfo(InPacket &in)
{
	ICQ_STR nick;
	in >> nick >> auth >> gender >> birth;

	nickName[MAX_NICK_LEN] = '\0';
	strncpy(nickName, nick.text, MAX_NICK_LEN);
}

void Session::onChangeStatus(InPacket &in)
{
	in >> status;
}

inline void Session::appendSearchResult(OutPacket &out)
{
	uint8 age = getAge(birth);

	out << userName << status << nickName;
	out << auth << gender << age;
}

void Session::onSearchRandom(InPacket &in)
{
	Session *sessions[MAX_SEARCH_RESULTS];
	int n = sessionManager.randomSessions(sessions, MAX_SEARCH_RESULTS);

	OutPacket out;
	out << (uint16) n;

	for (int i = 0; i < n; i++)
		sessions[i]->appendSearchResult(out);

	PACKET p;
	p.to = userName;
	p.from = "";
	p.cmd = CMD_SRV_SEARCH_RESULT;
	p.data = out.data;
	p.dataLen = out.getLength();
	client->deliver(&p);
}

void Session::onSearch(InPacket &in)
{
	ICQ_STR name, nick, email;
	in >> name >> nick >> email;

	// The seach fields can not be all NULL!
	if (!name.len && !nick.len && !email.len) {
		ICQ_LOG("illegal search condition\n");
		return;
	}

	DBConn *conn = sessionManager.dbConn;

	// Compose sql statement
	SQLStmt sql(conn);
	SQL_APPEND(sql, "SELECT name, nick, auth, gender, birth "
		"FROM user_tbl, user_basic WHERE user_tbl.id=user_basic.id AND ");

	if (name.len) {
		SQL_APPEND(sql, "name=");
		sql << name;
	} else {
		if (nick.len) {
			SQL_APPEND(sql, "nick=");
			sql << nick;
		}
		if (email.len) {
			SQL_APPEND(sql, " AND email=");
			sql << email;
		}
		SQL_APPEND(sql, " LIMIT 20");
	}

	DBResult *res;
	if (!conn->query(sql.getData(), sql.getLength()) ||
		!(res = conn->getResult()))
		return;

	// Fetch the result
	uint16 n = 0;
	OutPacket out;
	out.cursor += sizeof(n);

	DB_ROW row;
	while ((row = res->fetchRow()) != NULL) {
		const char *name = row[0];
		const char *nick = row[1];
		uint8 auth = atoi(row[2]);
		uint8 gender = atoi(row[3]);
		uint8 age = getAge(atol(row[4]));
		uint32 status;

		Session *s = sessionManager.getSession(name);
		if (!s || s->status == STATUS_INVIS)
			status = STATUS_OFFLINE;
		else
			status = s->status;

		out << name << status;
		out << nick << auth << gender << age;
		n++;
	}
	res->destroy();

	// Send it out
	*(uint16 *) out.data = htons(n);

	PACKET p;
	p.to = userName;
	p.from = "";
	p.cmd = CMD_SRV_SEARCH_RESULT;
	p.data = out.data;
	p.dataLen = out.getLength();
	client->deliver(&p);
}

void Session::handleRequest(InPacket &in)
{
	uint16 cmd;
	in >> cmd;

	switch (cmd) {
	case CMD_SEARCH_RANDOM:
		onSearchRandom(in);
		break;

	case CMD_SEARCH:
		onSearch(in);
		break;

	case CMD_CHANGE_STATUS:
		onChangeStatus(in);
		break;

	case CMD_UPDATE_USER_INFO:
		onUpdateUserInfo(in);
		break;
	}
}
