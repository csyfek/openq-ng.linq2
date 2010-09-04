#ifndef _C2S_H
#define _C2S_H

#include "servicemain.h"
#include "socket.h"
#include "sessionhash.h"
#include "dbmodule.h"
#include "loggerimpl.h"
#include "option.h"


/*
 * Socket that listens on real servers
 */
class ListenSocket : public Socket {
public:
	bool init();

	virtual bool onSocketRead();
};


/*
 * The main service class
 */
class C2S : public ServiceMain, public Socket {
public:
	C2S();
	~C2S();

	// Socket interface...
	virtual bool onSocketRead();

	// ServiceMain interface...
	virtual const char *getName() { return "linq_c2s"; }
	virtual bool init(int argc, char *argv[]);
	virtual void run();

	Session *getSession(uint32 ip, uint16 port) {
		return sessionHash->get(ip, port);
	}
	bool sendPacket(const char *data, int n, uint32 ip, uint16 port) {
		toAddr.sin_addr.s_addr = ip;
		toAddr.sin_port = port;
		return (sendto(sockfd, data, n, 0, (sockaddr *) &toAddr, sizeof(toAddr)) >= 0);
	}
	bool checkUserName(const char *name);
	void addServer(RealServer *server);
	void removeServer(RealServer *server);
	RealServer *chooseServer();

	time_t curTime;
	ListHead sessionList;
	ListHead sendQueue;

	Option option;
	SocketRegistryImpl socketRegistry;
	DBConn *dbConn;
	LoggerImpl *logger;

private:
	void initCharTable();
	bool initDB();
	bool initNet();

	void checkTimeouts() {
		checkSendQueue();
		checkKeepAlive();
	}

	void checkSendQueue();
	void checkKeepAlive();

	void onPacketReceived(const char *data, int n, sockaddr_in &addr);

	char legalCharTable[256];
	sockaddr_in toAddr;
	RealServer *serverList;
	RealServer *curServer;
	SessionHash *sessionHash;
	ListenSocket listenSocket;
	DBModule dbModule;
};


extern C2S c2s;


#endif
