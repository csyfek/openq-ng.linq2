#include "clientsession.h"
#include "icqmain.h"
#include "debug.h"


ClientSession::ClientSession(uint32 id, const char *name, uint32 ip, uint16 port)
{
	strncpy(userName, name, MAX_NAME_LEN);
	userName[MAX_NAME_LEN] = '\0';

	userID = id;
	clientIP = ip;
	clientPort = port;

	refCount = 1;
	queue = -1;		// Not assigned
}

void ClientSession::registerEvent(int module, void *data)
{
	// If no data, you should use MAPI::registerEvent
	ICQ_ASSERT(data != NULL);

	Module *m = icqMain.getModule(module);

	// Used when we stop this module
	m->hasSessionEvent = true;

	// Add it to an ordered list
	moduleList.add(&memPool, m, data);
}

// Deliver packet to client
void ClientSession::deliver(uint16 cmd, const char *data, int n)
{
	icqMain.c2s->deliver(clientIP, clientPort, cmd, data, n);
}
