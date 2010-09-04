#include "config.h"
#include "c2s.h"
#include "proto.h"
#include "debug.h"
#include "realserver.h"
#include "dbmodule.h"


C2S c2s;


bool ListenSocket::init()
{
	if (!createSocket(SOCK_STREAM))
		return false;

	if (!bindAddress(c2s.option.linqd_ip, c2s.option.linqd_port)) {
		ICQ_LOG("Can not bind on port %d\n", c2s.option.linqd_port);
		return false;
	}

	listen(sockfd, 5);

	addEvent(SOCKET_READ);
	return true;
}

bool ListenSocket::onSocketRead()
{
	int fd = accept(sockfd, NULL, NULL);
	if (fd < 0) {
		ICQ_LOG("accept() failed\n");
		return false;
	}

	ICQ_LOG("accept new server\n");

	// Accept a server and add it to the list.
	RealServer *server = new RealServer(fd);
	c2s.addServer(server);
	return true;
}


C2S::C2S()
{
	// This must be the first
	Socket::socketRegistry = &socketRegistry;

	curTime = 0;
	curServer = serverList = NULL;
	dbConn = NULL;

	initCharTable();

	sessionHash = new SessionHash;
	logger = new LoggerImpl;

	// Performance hack 8-)
	memset(&toAddr, 0, sizeof(toAddr));
	toAddr.sin_family = AF_INET;
}

C2S::~C2S()
{
	while (!sessionList.isEmpty()) {
		Session *s = LIST_ENTRY(sessionList.removeHead(), Session, listItem);
		delete s;
	}
	while (serverList) {
		RealServer *s = serverList;
		serverList = serverList->next;
		delete s;
	}

	if (dbConn)
		dbConn->destroy();

	delete sessionHash;
	delete logger;
}

/*
 * Used to check if user name is valid
 */
void C2S::initCharTable()
{
	memset(legalCharTable, 0, sizeof(legalCharTable));

	// alphanum
	char c;
	for (c = 'a'; c <= 'z'; c++)
		legalCharTable[c] = 1;
	for (c = 'A'; c <= 'Z'; c++)
		legalCharTable[c] = 1;
	for (c = '0'; c <= '9'; c++)
		legalCharTable[c] = 1;

	legalCharTable['-'] = 1;
	legalCharTable['_'] = 1;
}

/*
 * Initialize the db subsystem
 */
bool C2S::initDB()
{
	// Load the db module
	string name = LINQ_LIB_DIR "/db/";
	name += option.db_impl;

	if (!dbModule.load(name.c_str())) {
		ICQ_LOG("Can not load db module\n");
		return false;
	}

	// Create a new db connection
	dbConn = dbModule.getConn();
	if (!dbConn)
		return false;

	if (!dbConn->init(
		option.db_host.c_str(),
		option.db_user.c_str(),
		option.db_passwd.c_str(),
		option.db_name.c_str(),
		option.db_port))
		return false;

	return true;
}

/*
 * Initialize the net subsystem
 */
bool C2S::initNet()
{
	if (!createSocket(SOCK_DGRAM))
		return false;

	if (!bindAddress(option.c2s_ip, option.c2s_port)) {
		ICQ_LOG("Can not bind on port %d\n", option.c2s_port);
		return false;
	}

	addEvent(SOCKET_READ);

	return listenSocket.init();
}

bool C2S::init(int argc, char *argv[])
{
	srand(time(&curTime));

	// Load configuration
	option.load(argc, argv);

	ICQ_LOG("log_level=%d\n", option.log_level);
	logger->open(option.log_ip, option.log_port, option.log_level, "c2s");

	if (!initDB()) {
		logger->log(LOG_ERROR, "Can not initialize db subsystem");
		return false;
	}
	if (!initNet()) {
		logger->log(LOG_ERROR, "Can not initialize net subsystem");
		return false;
	}

	logger->log(LOG_INFORMATION, "c2s is now started");
	return true;
}

/*
 * Check if the user name is valid
 */
bool C2S::checkUserName(const char *name)
{
	for (; *name; name++) {
		if (!legalCharTable[*name])
			return false;
	}
	return true;
}

/*
 * Add a server to the list
 */
void C2S::addServer(RealServer *server)
{
	server->next = serverList;
	serverList = server;
}

/*
 * Remove a server from the list
 */
void C2S::removeServer(RealServer *server)
{
	// Update curServer if we remove it!
	if (curServer == server)
		curServer = server->next;

	// Delete all of the sessions associated with this server.
	ListHead *pos = sessionList.next;

	while (pos != &sessionList) {
		Session *s = LIST_ENTRY(pos, Session, listItem);

		if (s->realServer != server)
			pos = pos->next;
		else {
			ListHead *next = pos->next;
			delete s;
			pos = next;
		}
	}

	// Delete from the list
	RealServer *p = serverList;
	if (p == server)
		serverList = serverList->next;
	else {
		while (p && p->next != server)
			p = p->next;

		if (p)
			p->next = p->next->next;
	}

	delete server;
}

/*
 * Choose a server to route packet to
 *
 * TODO: Add load balancing support
 */
RealServer *C2S::chooseServer()
{
	// Round robin
	if (curServer)
		curServer = curServer->next;
	if (!curServer)
		curServer = serverList;

	return curServer;
}

void C2S::checkSendQueue()
{
	while (!sendQueue.isEmpty()) {
		C2S_PACKET *p = LIST_ENTRY(sendQueue.next, C2S_PACKET, globalSendItem);

		// The send-queue is sort by expiration time
		if (p->expiry > curTime)
			break;

		p->globalSendItem.remove();
		p->attempts--;

		if (p->attempts >= 0) {
			ICQ_LOG("Packet(seq=%d) timeout, resend it\n", p->seq);

			// Retry this packet
			p->session->deliverDirect(p);

			p->expiry = curTime + SEND_TIMEOUT;
			sendQueue.add(&p->globalSendItem);

		} else {
			ICQ_LOG("Packet %d failed\n", p->seq);

			// Maximum attempts reached, delete it
			p->sendItem.remove();
			free(p);
		}
	}
}

/*
 * Check if any sessions were expired
 */
void C2S::checkKeepAlive()
{
	while (!sessionList.isEmpty()) {
		Session *s = LIST_ENTRY(sessionList.next, Session, listItem);

		// Sort by expiration time
		if (s->expiry > curTime)
			break;

		logger->log(LOG_INFORMATION, "session %s expires", s->userName);
		delete s;
	}
}

void C2S::onPacketReceived(const char *data, int n, sockaddr_in &addr)
{
	// Any packet must have a complete header
	if (n < sizeof(C2S_HEADER)) {
		ICQ_LOG("Invalid packet header\n");
		return;
	}

	// Fill the header
	C2S_HEADER *header = (C2S_HEADER *) data;
	header->ver = ntohs(header->ver);
	header->sid = ntohl(header->sid);
	header->seq = ntohs(header->seq);
	header->cmd = ntohs(header->cmd);

	// Get a session from the hash by (ip, port)
	Session *session = getSession(addr.sin_addr.s_addr, addr.sin_port);

	if (!session) {
		// Only login and register packet can create a new session
		if (header->cmd != CMD_LOGIN && header->cmd != CMD_REGISTER) {
			ICQ_LOG("There is no such session.\n");
			return;
		}

		session = new Session(header);
		session->sessionIP = addr.sin_addr.s_addr;
		session->sessionPort = addr.sin_port;

		// Push it to the hash
		sessionHash->put(session);
	}

	// Compose a packet and process it
	InPacket in((char *) (header + 1), n - sizeof(C2S_HEADER));
	session->onPacketReceived(in, header);
}

bool C2S::onSocketRead()
{
	char buf[MAX_PACKET_SIZE];
	sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	int n = recvfrom(sockfd, buf, MAX_PACKET_SIZE, 0, (sockaddr *) &addr, &addrlen);
	if (n < 0) {
		// This may happen when destination host or port can not be reached
		ICQ_LOG("recvfrom() failed\n");
		return true;
	}

	onPacketReceived(buf, n, addr);
	return true;
}

void C2S::run()
{
	// Main event loop
	while (true) {
		time_t t;

		if (time(&t) != curTime) {
			curTime = t;
			checkTimeouts();
		}

		if (!socketRegistry.poll(curTime))
			break;
	}
}
