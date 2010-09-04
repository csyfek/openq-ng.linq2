#ifndef _SESSION_H
#define _SESSION_H

#include "types.h"
#include "pool.h"
#include <time.h>


/*
 * A session represents an online user
 */
class Session {
public:
	virtual const char *getUserName() = 0;
	// Get the unique id allocated automatically by db
	virtual uint32 getUserID() = 0;
	// IP Address of the user
	virtual uint32 getIP() = 0;
	// Get session's pool that we (including module) will allocate data from
	virtual Pool *getPool() = 0;

	// Register per-session event
	virtual void registerEvent(int module, void *data) = 0;
	// Deliver packet to user
	virtual void deliver(uint16 cmd, const char *data, int n) = 0;
};


#endif
