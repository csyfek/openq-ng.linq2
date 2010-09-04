#ifndef _RESOLVER_H
#define _RESOLVER_H

#include "tcpstream.h"


class Resolver : public TCPStream {
public:
	void resolve(const char *name);

private:
	virtual void onPacketReceived(const char *data, int n);
};


#endif
