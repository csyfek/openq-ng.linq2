#ifndef _CLIENT_H
#define _CLIENT_H

#include "tcpstream.h"
#include "logger.h"
#include "list.h"


class Client : public TCPStream {
public:
	Client(int fd);
	~Client();

	virtual void onPacketReceived(const char *data, int n);
	virtual void onSocketClose();

	ListHead list;

private:
	Logger *logger;
};


#endif
