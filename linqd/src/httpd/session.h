#ifndef _SESSION_H
#define _SESSION_H

#include "udpsocket.h"
#include "tcpstream.h"
#include "list.h"


class Session : public UDPListener, public TCPStream {
public:
	Session(int fd);
	~Session();

	bool connect(sockaddr_in &addr) {
		return udpSocket->connect(addr);
	}

	ListHead list;

private:
	virtual void onUDPPacket(const char *data, int n);
	virtual void onPacketReceived(const char *data, int n);
	virtual void onSocketClose();

	UDPSocket *udpSocket;
};


#endif
