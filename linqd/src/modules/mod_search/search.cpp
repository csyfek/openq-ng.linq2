#include "search.h"
#include "session.h"
#include "packet.h"
#include "sqlstmt.h"
#include "proto.h"
#include "debug.h"


Search::Search()
{
	sessionManager = NULL;
}

Search::~Search()
{
	if (sessionManager)
		delete sessionManager;
}

bool Search::onSessionEvent(EVENT *ev)
{
	InPacket in(ev->packet->data, ev->packet->dataLen);

	uint32 status;
	in >> status;

	SQLStmt sql(ev->conn);
	DBResult *res;

	SQL_APPEND(sql, "SELECT nick, auth, gender, birth FROM user_basic WHERE id=");
	sql << ev->s->getUserID();
	if (!ev->conn->query(sql.getData(), sql.getLength()) ||
		!(res = ev->conn->getResult()))
		return false;

	DB_ROW row = res->fetchRow();
	if (!row) {
		res->destroy();
		return false;
	}

	OutPacket out;
	out << status;

	uint8 auth = atoi(row[1]);
	uint8 gender = atoi(row[2]);
	uint32 birth = atol(row[3]);
	out << row[0] << auth << gender << birth;

	res->destroy();

	sessionManager->addSession(ev->s->getUserName(), out.data, out.getLength());
	return false;
}

bool Search::onSessionOut(EVENT *ev)
{
	uint16 cmd = ev->packet->cmd;

	if (cmd != CMD_SEARCH && cmd != CMD_SEARCH_RANDOM &&
		cmd != CMD_CHANGE_STATUS && cmd != CMD_UPDATE_USER_INFO)
		return false;

	// Redirect it to the session manager
	sessionManager->sendRequest(ev->s->getUserName(), ev->packet);

	return (cmd == CMD_SEARCH || cmd == CMD_SEARCH_RANDOM);
}

bool Search::onSessionIn(EVENT *ev)
{
	if (ev->packet->cmd != CMD_SRV_SEARCH_RESULT)
		return false;

	if (ev->s)
		ev->s->deliver(ev->packet->cmd, ev->packet->data, ev->packet->dataLen);
	return true;
}


bool Search::handleEvent(EVENT *ev, void *data)
{
	switch (ev->event) {
	case EV_DELIVER:
		return sessionManager->deliver(ev->packet);

	case EV_SESSION:
		return onSessionEvent(ev);

	case EV_S_IN:
		return onSessionIn(ev);

	case EV_S_OUT:
		return onSessionOut(ev);

	case EV_S_END:
		sessionManager->removeSession(ev->s->getUserName());
		break;
	}
	return false;
}

bool Search::init(int module, MAPI *mapi, Profile *prof)
{
	Socket::socketRegistry = mapi->getSocketRegistry();

	uint32 ip = inet_addr(prof->getString("sm_ip", "127.0.0.1"));
	if (ip == INADDR_NONE)
		return false;

	int port = prof->getInteger("sm_port", 8001);

	sessionManager = new SessionManager(mapi);
	if (!sessionManager->connect(ip, port, 20))
		return false;

	mapi->registerEvent(module, EV_SESSION);
	mapi->registerEvent(module, EV_S_OUT);
	mapi->registerEvent(module, EV_S_IN);
	mapi->registerEvent(module, EV_S_END);
	mapi->registerEvent(module, EV_DELIVER);
	return true;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new Search();
}
