#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include "socket.h"
#include "servicemain.h"
#include "sessionhash.h"
#include "dbmodule.h"
#include "option.h"


class S2S;

class SessionManager : public Socket, public ServiceMain {
public:
	SessionManager();
	~SessionManager();

	virtual bool onSocketRead();

	virtual const char *getName() { return "linq_sm"; }
	virtual bool init(int argc, char *argv[]);
	virtual void run();

	Client *chooseClient() { return clientList; }
	void removeClient(Client *c);
	void addSession(Session *s);
	Session *getSession(const char *name);
	void deliverPacket(InPacket &in);
	int randomSessions(Session *sessions[], int n) {
		return sessionHash.random(sessions, n);
	}

	SocketRegistryImpl socketRegistry;
	DBConn *dbConn;
	time_t curTime;

private:
	bool initDB();

	Option option;
	DBModule dbModule;
	SessionHash sessionHash;
	Client *clientList;
	S2S *s2s;
};


extern SessionManager sessionManager;


#endif
