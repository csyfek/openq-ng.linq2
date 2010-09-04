#include "resolver.h"
#include "s2s.h"
#include "debug.h"


void Resolver::resolve(const char *name)
{
	OutPacket out;
	out << name;

	sendPacket(out, NULL, 0);
}

void Resolver::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);

	ICQ_STR name;
	in >> name;
	uint32 ip = in.read32();

	Server *s = s2s.getServer(name.text);
	if (!s) {
		ICQ_LOG("No server %s\n", name.text);
		return;
	}

	s->onResolved(ip);
}
