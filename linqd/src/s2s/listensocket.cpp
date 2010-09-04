#include "listensocket.h"
#include "s2s.h"
#include "debug.h"


bool ListenSocket::listen(uint32 ip, int port)
{
	if (!createSocket(SOCK_STREAM))
		return false;

	if (!bindAddress(ip, port)) {
		ICQ_LOG("Can not bind on port %d\n", port);
		return false;
	}

	::listen(sockfd, 5);
	addEvent(SOCKET_READ);
	return true;
}

bool ListenSocket::onSocketRead()
{
	int fd = accept(sockfd, NULL, NULL);
	if (fd < 0)
		return false;

	Server *s = new Server(fd);
	s2s.serverList.add(&s->listItem);
	return true;
}
