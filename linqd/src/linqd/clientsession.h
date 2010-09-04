#ifndef _CLIENT_SESSION_H
#define _CLIENT_SESSION_H

#include "session.h"
#include "list.h"
#include "packet.h"
#include "event.h"
#include "poolimpl.h"
#include "modulelist.h"


#define MAX_NAME_LEN	16


class ClientSession : public Session {
public:
	ClientSession(uint32 id, const char *name, uint32 ip, uint16 port);

	// As you know, we are using multi-threading:-)
	void addRef() { refCount++; }
	void release() { if (!--refCount) delete this; }

	// Interfaces in Session ...
	virtual const char *getUserName() { return userName; }
	virtual uint32 getUserID() { return userID; }
	virtual uint32 getIP() { return clientIP; }
	virtual Pool *getPool() { return &memPool; }

	virtual void registerEvent(int module, void *data);
	virtual void deliver(uint16 cmd, const char *data, int n);

	char userName[MAX_NAME_LEN + 1];
	ModuleList moduleList;
	ListHead hashItem;
	int queue;	// The event queue currently assigned to us

private:
	// All session-specific data should be allocated from here
	PoolImpl memPool;

	uint32 userID;
	uint32 clientIP;
	uint16 clientPort;
	int refCount;
};


#endif
