#ifndef _LOG_APP_H
#define _LOG_APP_H

#include "socket.h"
#include "servicemain.h"
#include "list.h"
#include "option.h"
#include <stdio.h>


class LogApp : public Socket, public ServiceMain {
public:
	LogApp();
	~LogApp();

	virtual bool onSocketRead();

	virtual const char *getName() { return "linq_log"; }
	virtual bool init(int argc, char *argv[]);
	virtual void run();

	Option option;
	SocketRegistryImpl socketRegistry;

private:
	ListHead clientList;
	time_t curTime;
};


extern LogApp logApp;


#endif
