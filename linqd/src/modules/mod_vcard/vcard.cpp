#include "vcard.h"
#include "mapi.h"
#include "session.h"
#include "dbconn.h"
#include "packet.h"
#include "sqlstmt.h"
#include "proto.h"
#include <stdlib.h>


#define AUTH_NONE	0xff


struct USER_INFO {
	uint8 auth;
};

static int moduleIndex = -1;


static uint8 getUserAuth(USER_INFO *info, DBConn *conn, Session *s, const char *name)
{
	if (info && info->auth != AUTH_NONE)
		return info->auth;

	SQLStmt sql(conn);

	if (s) {
		SQL_APPEND(sql, "SELECT auth FROM user_basic WHERE id=");
		sql << s->getUserID();
	} else {
		SQL_APPEND(sql, "SELECT auth FROM user_basic, user_tbl "
			"WHERE user_tbl.id=user_basic.id AND name=");
		sql << name;
	}

	DBResult *res;
	if (!conn->query(sql.getData(), sql.getLength())||
		!(res = conn->getResult()))
		return AUTH_REJECT;

	DB_ROW row = res->fetchRow();
	uint8 auth = (row ? atoi(row[0]) : AUTH_REJECT);
	res->destroy();

	if (info)
		info->auth = auth;
	return auth;
}

static void onGetContactInfo(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	ICQ_STR name;
	in >> name;

	PACKET p;
	p.online = false;
	p.to = name.text;
	p.from = ev->s->getUserName();
	p.cmd = CMD_SRV_CONTACT_INFO;
	p.data = in.cursor;
	p.dataLen = in.getBytesLeft();

	ev->mapi->deliver(&p);
}

static void onGetUserInfo(EVENT *ev)
{
	SQLStmt sql(ev->conn);
	DBResult *res;
	
	SQL_APPEND(sql, "SELECT auth, nick, gender, birth, email,"
		"country, city, address, postcode, tel, mobile,"
		"realname, occupation, homepage, intro "
		"FROM user_tbl a, user_basic b, user_ext c "
		"WHERE a.id=b.id AND a.id=c.id AND a.id=");
	sql << ev->s->getUserID();

	if (!ev->conn->query(sql.getData(), sql.getLength()) ||
		!(res = ev->conn->getResult()))
		return;

	DB_ROW row = res->fetchRow();
	if (row) {
		uint8 auth = atoi(*row++);

		OutPacket out;
		out << auth << *row++;
		uint8 gender = atoi(*row++);
		uint32 birth = atol(*row++);
		out << gender << birth;
		for (int i = 0; i < 11; i++)
			out << *row++;

		ev->s->deliver(CMD_GET_USER_INFO, out.data, out.getLength());
	}

	res->destroy();
}

static void onUpdateUserInfo(EVENT *ev, USER_INFO *info)
{
	uint8 auth, gender;
	uint32 birth;
	ICQ_STR nick, email, country, city, address;
	ICQ_STR postcode, tel, mobile;
	ICQ_STR realname, occupation, homepage, intro;

	InPacket in(ev->packet->data, ev->packet->dataLen);

	in >> auth >> nick >> gender >> birth >> email;
	in >> country >> city >> address >> postcode >> tel >> mobile;
	in >> realname >> occupation >> homepage >> intro;

	SQLStmt sql(ev->conn);
	uint32 id = ev->s->getUserID();

	// Basic information
	SQL_APPEND(sql, "UPDATE user_basic SET auth=");
	sql << (uint32) auth;
	SQL_APPEND(sql, ",nick=");
	sql << nick;
	SQL_APPEND(sql, ",gender=");
	sql << (uint32) gender;
	SQL_APPEND(sql, ",birth=");
	sql << birth;
	SQL_APPEND(sql, ",email=");
	sql << email;
	SQL_APPEND(sql, " WHERE id=");
	sql << id;

	ev->conn->query(sql.getData(), sql.getLength());

	// Ext Information
	sql.reset();

	SQL_APPEND(sql, "UPDATE user_ext SET country=");
	sql << country;
	SQL_APPEND(sql, ",city=");
	sql << city;
	SQL_APPEND(sql, ",address=");
	sql << address;
	SQL_APPEND(sql, ",postcode=");
	sql << postcode;
	SQL_APPEND(sql, ",tel=");
	sql << tel;
	SQL_APPEND(sql, ",mobile=");
	sql << mobile;
	SQL_APPEND(sql, ",realname=");
	sql << realname;
	SQL_APPEND(sql, ",occupation=");
	sql << occupation;
	SQL_APPEND(sql, ",homepage=");
	sql << homepage;
	SQL_APPEND(sql, ",intro=");
	sql << intro;
	SQL_APPEND(sql, " WHERE id=");
	sql << id;

	ev->conn->query(sql.getData(), sql.getLength());

	info->auth = auth;
}

static void onChangePasswd(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	ICQ_STR passwd;
	in >> passwd;
	if (passwd.len != 32)
		return;

	SQLStmt sql(ev->conn);

	SQL_APPEND(sql, "UPDATE user_tbl SET passwd=");
	sql << passwd;
	SQL_APPEND(sql, " WHERE id=");
	sql << ev->s->getUserID();

	ev->conn->query(sql.getData(), sql.getLength());
}

static bool onSessionEvent(EVENT *ev)
{
	USER_INFO *info = (USER_INFO *) ev->s->getPool()->alloc(sizeof(USER_INFO));
	info->auth = AUTH_NONE;

	ev->s->registerEvent(moduleIndex, info);
	return false;
}

static bool onSessionOut(EVENT *ev, USER_INFO *info)
{
	switch (ev->packet->cmd) {
	case CMD_GET_CONTACT_INFO:
		onGetContactInfo(ev);
		break;

	case CMD_GET_USER_INFO:
		onGetUserInfo(ev);
		break;

	case CMD_UPDATE_USER_INFO:
		onUpdateUserInfo(ev, info);
		break;

	case CMD_CHANGE_PASSWD:
		onChangePasswd(ev);
		break;

	default:
		return false;
	}

	return true;
}

static bool onAddContactIn(EVENT *ev, USER_INFO *info)
{
	uint8 auth = getUserAuth(info, ev->conn, ev->s, ev->packet->to);

	OutPacket out;
	out << auth;

	PACKET p;
	p.cmd = CMD_SRV_CONTACT_AUTH;
	p.from = ev->packet->to;
	p.to = ev->packet->from;
	p.data = out.data;
	p.dataLen = out.getLength();
	p.online = true;

	ev->mapi->deliverFrom(&p);
	return true;
}

static void onContactInfoIn(EVENT *ev)
{
	SQLStmt sql(ev->conn);
	DBResult *res;

	SQL_APPEND(sql, "SELECT nick, gender, birth, email,"
		"country, city, address, postcode, tel, mobile,"
		"realname, occupation, homepage, intro "
		"FROM user_tbl a, user_basic b, user_ext c "
		"WHERE a.id=b.id AND a.id=c.id AND name=");
	sql << ev->packet->to;

	if (!ev->conn->query(sql.getData(), sql.getLength()) ||
		!(res = ev->conn->getResult()))
		return;

	DB_ROW row = res->fetchRow();
	if (row) {
		OutPacket out;

		out << *row++;
		uint8 gender = atoi(*row++);
		uint32 birth = atol(*row++);
		out << gender << birth;
		for (int i = 0; i < 11; i++)
			out << *row++;

		PACKET p;
		p.online = true;
		p.cmd = CMD_SRV_CONTACT_INFO_REPLY;
		p.from = ev->packet->to;
		p.to = ev->packet->from;
		p.data = out.data;
		p.dataLen = out.getLength();

		ev->mapi->deliver(&p);
	}

	res->destroy();
}

static void onContactInfoReply(EVENT *ev)
{
	OutPacket out;
	out << ev->packet->from;
	out.appendData(ev->packet->data, ev->packet->dataLen);

	ev->s->deliver(CMD_GET_CONTACT_INFO, out.data, out.getLength());
}

static bool onSessionIn(EVENT *ev, USER_INFO *info)
{
	switch (ev->packet->cmd) {
	case CMD_SRV_ADD_CONTACT:
		onAddContactIn(ev, info);
		break;

	case CMD_SRV_CONTACT_INFO:
		onContactInfoIn(ev);
		break;

	case CMD_SRV_CONTACT_INFO_REPLY:
		onContactInfoReply(ev);
		break;

	default:
		return false;
	}
	return true;
}


bool VCard::handleEvent(EVENT *ev, void *data)
{
	USER_INFO *info = (USER_INFO *) data;

	switch (ev->event) {
	case EV_SESSION:
		return onSessionEvent(ev);

	case EV_S_OUT:
		return onSessionOut(ev, info);

	case EV_S_IN:
		return onSessionIn(ev, info);
	}
	return false;
}

bool VCard::init(int module, MAPI *mapi, Profile *prof)
{
	moduleIndex = module;

	mapi->registerEvent(module, EV_SESSION);
	mapi->registerEvent(module, EV_S_IN);
	return true;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new VCard;
}
