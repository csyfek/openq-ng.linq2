#include "thread.h"
#include "debug.h"


static void *threadFunc(void *arg)
{
	((Thread *) arg)->run();
	return NULL;
}


Thread::Thread()
{
	thread = 0;
}

Thread::~Thread()
{
}

bool Thread::start()
{
	return (pthread_create(&thread, NULL, threadFunc, this) == 0);
}

void Thread::wait()
{
	pthread_join(thread, NULL);
}
