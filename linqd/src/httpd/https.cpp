#include "https.h"
#include "session.h"
#include "debug.h"


Https https;

static sockaddr_in serverAddr;


Https::Https()
{
	Socket::socketRegistry = &socketRegistry;

	memset(&serverAddr, 0, sizeof(serverAddr));
}

Https::~Https()
{
	while (!sessionList.isEmpty()) {
		Session *s = LIST_ENTRY(sessionList.removeHead(), Session, list);
		delete s;
	}
}

bool Https::init(int argc, char *argv[])
{
	option.load(argc, argv);

	if (!createSocket(SOCK_STREAM))
		return false;

	if (!bindAddress(option.https_ip, option.https_port)) {
		ICQ_LOG("Can not bind on port %d\n", option.https_port);
		return false;
	}

	listen(sockfd, 5);
	addEvent(SOCKET_READ);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = option.c2s_ip;
	serverAddr.sin_port = htons(option.c2s_port);

	ICQ_LOG("httpd is now started\n");
	return true;
}

bool Https::onSocketRead()
{
	int fd = accept(sockfd, NULL, NULL);
	if (fd < 0)
		return false;

	ICQ_LOG("Accept new client\n");

	Session *s = new Session(fd);
	if (s->connect(serverAddr))
		sessionList.add(&s->list);
	else
		delete s;

	return true;
}

void Https::run()
{
	time_t t;
	while (socketRegistry.poll(time(&t)))
		;
}
