#ifndef _MAPI_H
#define _MAPI_H

#include "event.h"
#include <time.h>

class DBConn;
class Logger;
class SocketRegistry;


/*
 * Module APIs
 */
class MAPI {
public:
	// Get the current time, performance hack, I think ;-)
	virtual time_t getTime() = 0;

	virtual Logger *getLogger() = 0;
	virtual SocketRegistry *getSocketRegistry() = 0;

	// Register a module event. If you want per-session event,
	// use Session::registerEvent() instead.
	virtual void registerEvent(int module, int event) = 0;

	// Deliver packet from a session (send EV_S_OUT event)
	virtual void deliverFrom(PACKET *p) = 0;
	// Deliver packet to a session
	virtual void deliver(PACKET *p) = 0;
	// Deliver packet locally (not sent to sm)
	virtual void deliverLocal(PACKET *p) = 0;

	// Module management
	virtual bool startModule(const char *name) = 0;
	virtual bool stopModule(const char *name) = 0;
	virtual void getModuleList(char *buf, int n) = 0;
};


#endif
