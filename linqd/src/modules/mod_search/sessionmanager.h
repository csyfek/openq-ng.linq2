#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include "tcpstream.h"
#include "mapi.h"


class SessionManager : public TCPStream {
public:
	SessionManager(MAPI *m);

	void addSession(const char *name, const char *data, int n);
	void removeSession(const char *name);
	bool deliver(PACKET *p);
	void sendRequest(const char *from, PACKET *p);

private:
	virtual void onPacketReceived(const char *data, int n);

	MAPI *mapi;
};


#endif
