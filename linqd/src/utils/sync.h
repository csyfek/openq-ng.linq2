#ifndef _SYNC_H
#define _SYNC_H

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <semaphore.h>
#endif


class Mutex {
public:
	Mutex();
	~Mutex();

	void lock();
	void unlock();

private:

#ifdef _WIN32
	CRITICAL_SECTION cs;
#else
	pthread_mutex_t m;
#endif
};


class Semaphore {
public:
	Semaphore();
	~Semaphore();

	void down();
	void up();

private:
#ifdef _WIN32
	HANDLE sem;
#else
	sem_t sem;
#endif
};


// Implementation
#ifdef _WIN32
#include "win32\sync.cpp"
#else
#include "linux/sync.cpp"
#endif

#endif
