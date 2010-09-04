#ifndef _UDP_SOCKET_H
#define _UDP_SOCKET_H

#include "socket.h"

#ifdef _WIN32
#include <winsock.h>
#endif


class UDPListener {
public:
	virtual void onUDPPacket(const char *data, int n) = 0;
};


class UDPSocket : public Socket {
public:
	UDPSocket(UDPListener *l);

	bool connect(sockaddr_in &addr);
	void sendPacket(const char *data, int n);

private:
	virtual bool onSocketRead();

	UDPListener *listener;
};


#endif
