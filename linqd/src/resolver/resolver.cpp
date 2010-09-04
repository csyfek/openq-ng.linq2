#include "resolver.h"
#include "client.h"
#include "debug.h"


static Resolver resolver;


Resolver::Resolver()
{
	Socket::socketRegistry = &socketRegistry;
}

Resolver::~Resolver()
{
	while (!clientList.isEmpty()) {
		Client *c = LIST_ENTRY(clientList.removeHead(), Client, list);
		delete c;
	}
}

bool Resolver::init(int argc, char *argv[])
{
	option.load(argc, argv);

	if (!createSocket(SOCK_STREAM))
		return false;

	if (!bindAddress(option.ip, option.port))
		return false;

	listen(sockfd, 5);
	addEvent(SOCKET_READ);

	ICQ_LOG("named is now started\n");
	return true;
}

bool Resolver::onSocketRead()
{
	int fd = accept(sockfd, NULL, NULL);
	if (fd < 0)
		return false;

	Client *c = new Client(fd);
	clientList.add(&c->list);
	return true;
}

void Resolver::run()
{
	while (socketRegistry.poll(time(NULL)))
		;
}
