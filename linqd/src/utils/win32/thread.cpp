#include "thread.h"
#include "debug.h"


static DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	Thread *t = (Thread *) lpParameter;
	return t->run();
}


Thread::Thread()
{
	hThread = NULL;
}

Thread::~Thread()
{
	if (hThread)
		CloseHandle(hThread);
}

bool Thread::start()
{
	DWORD id;

	hThread = CreateThread(NULL, 0, ThreadProc, this, 0, &id);
	return (hThread != NULL);
}

void Thread::wait()
{
	ICQ_ASSERT(hThread != NULL);

	ICQ_ASSERT(WaitForSingleObject(hThread, INFINITE) != WAIT_FAILED);
}
