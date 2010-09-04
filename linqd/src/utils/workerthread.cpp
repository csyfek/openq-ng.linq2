#include "workerthread.h"
#include "debug.h"


/*
 * A quit signal
 */
class ExitItem : public Runnable {
public:
	bool run(Thread *t) { return false; }
};


BlockingQueue::BlockingQueue()
{
	head = tail = NULL;
}

BlockingQueue::~BlockingQueue()
{
	while (head) {
		Runnable *r = head;
		head = head->next;
		delete r;
	}
}

Runnable *BlockingQueue::get()
{
	sem.down();
	mutex.lock();

	ICQ_ASSERT(head != NULL);
	ICQ_ASSERT(tail != NULL);

	Runnable *r = head;
	if (!(head = head->next))
		tail = NULL;

	mutex.unlock();

	return r;
}

/*
 * Add an item to the end of the queue
 */
void BlockingQueue::add(Runnable *r)
{
	r->next = NULL;

	mutex.lock();

	if (tail)
		tail->next = r;
	else
		head = r;

	tail = r;

	mutex.unlock();
	sem.up();
}


void WorkerThread::exit()
{
	// Send a quit 'signal', and wait it to terminate
	addTask(new ExitItem);
	wait();
}

int WorkerThread::run()
{
	bool cont = true;

	while (cont) {
		Runnable *r = queue.get();
		cont = r->run(this);
		delete r;
	}

	ICQ_LOG("Received quit event!\n");
	return 0;
}
