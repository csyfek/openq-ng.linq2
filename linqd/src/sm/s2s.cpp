#include "s2s.h"
#include "sessionmanager.h"


void S2S::deliver(PACKET *p)
{
	OutPacket out;
	out << p->to << p->from << p->online << p->cmd;
	sendPacket(out, p->data, p->dataLen);
}

void S2S::onPacketReceived(const char *data, int n)
{
	InPacket in(data, n);

	sessionManager.deliverPacket(in);
}
