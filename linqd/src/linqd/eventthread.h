#ifndef _EVENT_THREAD_H
#define _EVENT_THREAD_H

#pragma warning(disable:4786)


#include "workerthread.h"
#include "clientsession.h"
#include "event.h"
#include "dbmodule.h"
#include "option.h"
#include "poolimpl.h"


// A task in the event queue
class EventItem : public Runnable {
public:
	EventItem(int e, ClientSession *s, PACKET *p);

	virtual bool run(Thread *t);

	int event;
	ClientSession *session;
	PACKET *packet;
	PoolImpl memPool;
};


class EventThread : public WorkerThread {
public:
	EventThread();
	~EventThread();

	bool init(DBModule *db, DB_INFO *info);

	// Every thread has its own db connection to keep it multi-threaded safe
	DBConn *dbConn;
};


#endif
