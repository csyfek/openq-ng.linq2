#ifndef _S2S_H
#define _S2S_H

#include "tcpstream.h"
#include "event.h"


class S2S : public TCPStream {
public:
	void deliver(PACKET *p);

	virtual void onPacketReceived(const char *data, int n);
};


#endif
