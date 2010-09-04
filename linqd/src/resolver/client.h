#ifndef _CLIENT_H
#define _CLIENT_H

#include "tcpstream.h"
#include "list.h"


class Client : public TCPStream {
public:
	Client(int fd);
	~Client();

	ListHead list;

private:
	virtual void onPacketReceived(const char *data, int n);
};


#endif
