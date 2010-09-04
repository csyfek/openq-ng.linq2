#include "config.h"
#include "sessionmanager.h"
#include "client.h"
#include "s2s.h"
#include "debug.h"


SessionManager sessionManager;


SessionManager::SessionManager()
{
	Socket::socketRegistry = &socketRegistry;

	clientList = NULL;
	s2s = new S2S;
}

SessionManager::~SessionManager()
{
	delete s2s;

	if (dbConn)
		dbConn->destroy();

	while (clientList) {
		Client *c = clientList;
		clientList = clientList->next;
		delete c;
	}
}

bool SessionManager::initDB()
{
	string name = LINQ_LIB_DIR"/db/";
	name += option.db_impl;

	if (!dbModule.load(name.c_str())) {
		ICQ_LOG("Can not load db module: %s\n", name.c_str());
		return false;
	}

	dbConn = dbModule.getConn();
	if (!dbConn)
		return false;

	DB_INFO *info = &option.db_info;
	if (!dbConn->init(
		info->host.c_str(),
		info->user.c_str(),
		info->passwd.c_str(),
		info->db.c_str(),
		info->port))
		return false;

	return true;
}

bool SessionManager::init(int argc, char *argv[])
{
	srand(time(&curTime));

	option.load(argc, argv);

	if (!initDB())
		return false;

	s2s->connect(option.s2s_ip, option.s2s_port, 30);

	if (!createSocket(SOCK_STREAM))
		return false;

	if (!bindAddress(option.sm_ip, option.sm_port)) {
		ICQ_LOG("Can not bind on port %d\n", option.sm_port);
		return false;
	}

	listen(sockfd, 5);
	addEvent(SOCKET_READ);

	ICQ_LOG("sm is now started\n");
	return true;
}

void SessionManager::run()
{
	while (true) {
		curTime = time(NULL);

		if (!socketRegistry.poll(curTime))
			break;
	}
}

void SessionManager::deliverPacket(InPacket &in)
{
	ICQ_STR to, from;
	uint8 online;
	uint16 cmd;

	in >> to >> from >> online >> cmd;

	PACKET p;
	p.to = to.text;
	p.from = from.text;
	p.cmd = cmd;
	p.online = online;
	p.data = in.cursor;
	p.dataLen = in.getBytesLeft();

	if (strchr(to.text, '@')) {
		s2s->deliver(&p);
		return;
	}

	Session *s = sessionHash.get(to.text);
	Client *client = NULL;

	if (s)
		client = s->client;
	else if (!online) {
		ICQ_LOG("%s is offline, bounce it\n", to.text);
		client = chooseClient();
	}

	if (client)
		client->deliver(&p);
}

void SessionManager::removeClient(Client *c)
{
	Client *p = clientList;
	if (p == c)
		clientList = clientList->next;
	else {
		while (p && p->next != c)
			p = p->next;
		if (p)
			p->next = p->next->next;
	}
	delete c;
}

void SessionManager::addSession(Session *s)
{
	ICQ_LOG("%s is added\n", s->userName);

	sessionHash.put(s);
}

Session *SessionManager::getSession(const char *name)
{
	return sessionHash.get(name);
}

bool SessionManager::onSocketRead()
{
	int fd = accept(sockfd, NULL, NULL);
	if (fd < 0)
		return false;

	Client *c = new Client(fd);

	ICQ_LOG("accept new server\n");

	c->next = clientList;
	clientList = c;
	return true;
}
