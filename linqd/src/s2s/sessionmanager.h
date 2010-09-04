#ifndef _SESSION_MANAGER_H
#define _SESSION_MANAGER_H

#include "tcpstream.h"
#include "list.h"
#include "event.h"


class SessionManager : public TCPStream {
public:
	SessionManager(int fd);

	void deliver(PACKET *p);

private:
	virtual void onPacketReceived(const char *data, int n);
	virtual void onSocketClose();
};


#endif
