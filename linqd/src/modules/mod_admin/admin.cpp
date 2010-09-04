#include "admin.h"
#include "client.h"
#include "logger.h"


Admin::Admin()
{
}

Admin::~Admin()
{
	while (!clientList.isEmpty()) {
		Client *c = LIST_ENTRY(clientList.removeHead(), Client, list);
		delete c;
	}
}

bool Admin::onSocketRead()
{
	int fd = accept(sockfd, NULL, NULL);
	if (fd < 0)
		return false;

	Client *c = new Client(mapi, fd);
	clientList.add(&c->list);
	return true;
}

bool Admin::init(int module, MAPI *api, Profile *prof)
{
	mapi = api;
	Socket::socketRegistry = mapi->getSocketRegistry();

	if (!createSocket(SOCK_STREAM))
		return false;

	uint32 ip = prof->getIP("ip", "127.0.0.1");
	int port = prof->getInteger("port", 23);

	if (!bindAddress(ip, port)) {
		mapi->getLogger()->log(LOG_ERROR, "Admin: Can not bind on port %d\n", port);
		return false;
	}

	listen(sockfd, 5);

	addEvent(SOCKET_READ);
	return true;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new Admin;
}
