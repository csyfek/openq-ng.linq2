#include "session.h"
#include "https.h"
#include "debug.h"


Session::Session(int fd) : TCPStream(fd)
{
	udpSocket = new UDPSocket(this);
}

Session::~Session()
{
	delete udpSocket;
}

void Session::onUDPPacket(const char *data, int n)
{
	OutPacket out;
	sendPacket(out, data, n);
}

void Session::onPacketReceived(const char *data, int n)
{
	udpSocket->sendPacket(data, n);
}

void Session::onSocketClose()
{
	ICQ_LOG("Client closed\n");

	list.remove();
	delete this;
}
