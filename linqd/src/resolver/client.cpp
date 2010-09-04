#include "client.h"
#include "debug.h"

#ifdef _WIN32
#include <winsock.h>
#else
#include <netdb.h>
#endif


Client::Client(int fd) : TCPStream(fd)
{
}

Client::~Client()
{
	list.remove();
}

void Client::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);
	ICQ_STR host;
	in >> host;

	ICQ_LOG("Resolving %s...\n", host.text);

	uint32 ip = INADDR_NONE;
	hostent *hent = gethostbyname(host.text);
	if (hent) {
		in_addr addr = *(in_addr *) hent->h_addr;
		ICQ_LOG("%s\n", inet_ntoa(addr));

		ip = addr.s_addr;
	}

	OutPacket out;
	out << host;
	out.write32(ip);

	sendPacket(out, NULL, 0);
}
