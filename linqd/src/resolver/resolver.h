#ifndef _RESOLVER_H
#define _RESOLVER_H

#include "socket.h"
#include "servicemain.h"
#include "list.h"
#include "option.h"


class Resolver : public Socket, public ServiceMain {
public:
	Resolver();
	~Resolver();

	virtual bool onSocketRead();

	virtual const char *getName() { return "linq_named"; }
	virtual bool init(int argc, char *argv[]);
	virtual void run();

private:
	Option option;
	ListHead clientList;
	SocketRegistryImpl socketRegistry;
};


#endif
