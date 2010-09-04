#ifndef _WORKER_THREAD_H
#define _WORKER_THREAD_H

#include "thread.h"
#include "sync.h"


/*
 * A task in the queue
 */
class Runnable {
friend class BlockingQueue;

public:
	// virtual destructor so that we can delete it
	virtual ~Runnable() {}

	// Returns false if it receives a 'quit' signal
	virtual bool run(Thread *t) = 0;

private:
	// Points to the next item, should it be there?
	Runnable *next;
};

class BlockingQueue {
public:
	BlockingQueue();
	~BlockingQueue();

	// Fetch an item from the queue. Maybe blocked
	Runnable *get();
	void add(Runnable *r);

private:
	Mutex mutex;
	Semaphore sem;

	Runnable *head, *tail;
};


/*
 * A thread that continously reads an item from a blocking queue, and process them
 */
class WorkerThread : public Thread {
public:
	// Append a new task to the queue to be run later.
	void addTask(Runnable *r) { queue.add(r); }

	// Wait and exit
	void exit();

private:
	virtual int run();

	BlockingQueue queue;
};


#endif
