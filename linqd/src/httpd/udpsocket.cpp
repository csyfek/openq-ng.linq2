#include "udpsocket.h"
#include "https.h"


UDPSocket::UDPSocket(UDPListener *l)
{
	listener = l;
}

bool UDPSocket::connect(sockaddr_in &addr)
{
	if (!createSocket(SOCK_DGRAM))
		return false;

	if (::connect(sockfd, (sockaddr *) &addr, sizeof(addr)) < 0)
		return false;

	addEvent(SOCKET_READ);
	return true;
}

void UDPSocket::sendPacket(const char *data, int n)
{
	send(sockfd, data, n, 0);
}

bool UDPSocket::onSocketRead()
{
	char buf[1024];
	int n = recv(sockfd, buf, sizeof(buf), 0);
	if (n <= 0)
		return false;

	listener->onUDPPacket(buf, n);
	return true;
}
