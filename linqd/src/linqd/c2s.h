#ifndef _C2S_H
#define _C2S_H

#include "tcpstream.h"
#include "packet.h"


class C2S : public TCPStream {
public:
	bool init();
	// Deliver packet to c2s by (ip, port)
	void deliver(uint32 ip, uint16 port, uint16 cmd, const char *data, int n);

private:
	virtual void onPacketReceived(const char *data, int n);

	void onAddSession(const char *name, InPacket &in);
	void onRemoveSession(const char *name, InPacket &in);
	void onDeliverPacket(const char *name, InPacket &in);
};


#endif
