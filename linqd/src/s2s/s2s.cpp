#include "s2s.h"
#include "debug.h"


S2S s2s;


S2S::S2S()
{
	Socket::socketRegistry = &socketRegistry;

	sessionManager = NULL;
	listenSocket = new ListenSocket;
	resolver = new Resolver;
}

S2S::~S2S()
{
	delete listenSocket;
	delete resolver;

	if (sessionManager)
		delete sessionManager;
}

bool S2S::init(int argc, char *argv[])
{
	option.load(argc, argv);

	resolver->connect(option.resolver_ip, option.resolver_port, 30);
	listenSocket->listen(option.s2s_ip, option.s2s_port);

	if (!createSocket(SOCK_STREAM))
		return false;

	if (!bindAddress(option.sm_ip, option.sm_port)) {
		ICQ_LOG("Can not bind on port %d\n", option.sm_port);
		return false;
	}

	listen(sockfd, 5);
	addEvent(SOCKET_READ);

	ICQ_LOG("s2s is now started\n");
	return true;
}

void S2S::removeClient(SessionManager *c)
{
	ICQ_ASSERT(sessionManager == c);

	delete sessionManager;
	sessionManager = NULL;
}

bool S2S::onSocketRead()
{
	if (sessionManager)
		return true;
	
	int fd = accept(sockfd, NULL, NULL);
	if (fd < 0)
		return false;

	sessionManager = new SessionManager(fd);
	return true;
}

void S2S::run()
{
	time_t t;

	while (true) {
		time(&t);

		if (!socketRegistry.poll(t))
			break;
	}
}
