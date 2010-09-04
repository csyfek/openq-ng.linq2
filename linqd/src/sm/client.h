#ifndef _CLIENT_H
#define _CLIENT_H

#include "tcpstream.h"
#include "list.h"
#include "event.h"


class Client : public TCPStream {
public:
	Client(int fd);
	~Client();

	virtual void onSocketClose();

	void deliver(PACKET *p);

	Client *next;

private:
	virtual void onPacketReceived(const char *data, int n);

	void onAddSession(InPacket &in);
	void onRemoveSession(InPacket &in);
	void onDeliver(InPacket &in);
	void onRequest(InPacket &in);

	ListHead sessionList;
};


#endif
