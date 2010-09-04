#ifndef _SERVER_H
#define _SERVER_H

#define MAX_SERVER_LEN		128

#include "tcpstream.h"
#include "list.h"
#include "event.h"


class Server : public TCPStream {
public:
	Server(const char *name);
	Server(int fd);
	~Server();

	void deliver(PACKET *p);
	void onResolved(uint32 ip);

	ListHead hashItem;
	ListHead listItem;
	char serverName[MAX_SERVER_LEN + 1];

private:
	virtual void onPacketReceived(const char *data, int n);
	virtual void onSocketConnect();
	virtual void onSocketTimeout();
	virtual void onSocketClose();

	void sendPacket(uint16 cmd, const char *data, int n);
	void onDeliver(InPacket &in);
	void onHello(InPacket &in);
	void onHelloReply(InPacket &in);
	void onPacketReceived(InPacket &in, uint16 cmd);

	uint32 serverIP;
	int status;
	ListHead packetList;
};


#endif
