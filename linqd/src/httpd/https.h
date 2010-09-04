#ifndef _HTTPS_H
#define _HTTPS_H

#include "socket.h"
#include "servicemain.h"
#include "list.h"
#include "option.h"


class Https : public Socket, public ServiceMain {
public:
	Https();
	~Https();

	virtual const char *getName() { return "linq_https"; }
	virtual bool init(int argc, char *argv[]);
	virtual void run();

	SocketRegistryImpl socketRegistry;

private:
	virtual bool onSocketRead();

	ListHead sessionList;
	Option option;
};


extern Https https;


#endif
