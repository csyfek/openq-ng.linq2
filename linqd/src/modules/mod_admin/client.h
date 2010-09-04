#ifndef _CLIENT_H
#define _CLIENT_H

#include "socket.h"
#include "list.h"
#include "mapi.h"


#define MAX_CMD_LEN		128


class Client : public Socket {
public:
	Client(MAPI *api, int fd);

	ListHead list;

private:
	virtual bool onSocketRead();

	void print(const char *fmt, ...);
	void printBanner();
	void printPrompt();
	void processCmd(char *line);
	void onHelp();
	void onExit();
	void onError();
	void onListModules();
	void onStartModule();
	void onStopModule();

	MAPI *mapi;
	char recvBuf[MAX_CMD_LEN];
	int bufLen;
};


#endif
