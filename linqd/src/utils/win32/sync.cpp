inline Mutex::Mutex()
{
	InitializeCriticalSection(&cs);
}

inline Mutex::~Mutex()
{
	DeleteCriticalSection(&cs);
}

inline void Mutex::lock()
{
	EnterCriticalSection(&cs);
}

inline void Mutex::unlock()
{
	LeaveCriticalSection(&cs);
}


inline Semaphore::Semaphore()
{
	sem = CreateSemaphore(NULL, 0, 4096, NULL);
}

inline Semaphore::~Semaphore()
{
	if (sem)
		CloseHandle(sem);
}

inline void Semaphore::down()
{
	WaitForSingleObject(sem, INFINITE);
}

inline void Semaphore::up()
{
	ReleaseSemaphore(sem, 1, NULL);
}
