#ifndef _THREAD_H
#define _THREAD_H

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif


class Thread {
public:
	Thread();
	~Thread();

	bool start();

	// Wait until this thread terminates
	void wait();

	virtual int run() = 0;

private:
#ifdef _WIN32
	HANDLE hThread;
#else
	pthread_t thread;
#endif
};


#endif
