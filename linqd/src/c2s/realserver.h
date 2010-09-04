#ifndef _REAL_SERVER_H
#define _REAL_SERVER_H

#include "tcpstream.h"

class Session;


class RealServer : public TCPStream {
public:
	RealServer(int fd);

	// Deliver packet to real server, from session
	void deliver(Session *s, uint16 cmd, const char *data, int n);
	// Add a new session on this server
	void addSession(Session *s, const char *data, int n);
	// Remove a session from this server
	void removeSession(Session *s);

	// Points to the next server in a linked list
	RealServer *next;

private:
	virtual void onPacketReceived(const char *data, int n);
	virtual void onSocketClose();
};


#endif
