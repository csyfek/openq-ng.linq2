#ifndef _S2S_H
#define _S2S_H

#include "socket.h"
#include "servicemain.h"
#include "option.h"
#include "list.h"

#include "serverhash.h"
#include "resolver.h"
#include "listensocket.h"
#include "sessionmanager.h"


class S2S : public Socket, public ServiceMain {
public:
	S2S();
	~S2S();

	virtual bool onSocketRead();

	virtual const char *getName() { return "linq_s2s"; }
	virtual bool init(int argc, char *argv[]);
	virtual void run();

	Server *getServer(const char *name) {
		return serverHash.get(name);
	}
	void addServer(Server *s) {
		serverHash.put(s);
	}
	void removeClient(SessionManager *c);

	SocketRegistryImpl socketRegistry;
	Resolver *resolver;
	Option option;

	ListHead serverList;
	ServerHash serverHash;
	SessionManager *sessionManager;

private:
	ListenSocket *listenSocket;
};


extern S2S s2s;


#endif
