#ifndef _LISTEN_SOCKET_H
#define _LISTEN_SOCKET_H

#include "socket.h"
#include "types.h"


class ListenSocket : public Socket {
public:
	bool listen(uint32 ip, int port);

private:
	virtual bool onSocketRead();
};


#endif
