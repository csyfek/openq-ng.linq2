inline Mutex::Mutex()
{
	pthread_mutex_init(&m, NULL);
}

inline Mutex::~Mutex()
{
	pthread_mutex_destroy(&m);
}

inline void Mutex::lock()
{
	pthread_mutex_lock(&m);
}

inline void Mutex::unlock()
{
	pthread_mutex_unlock(&m);
}


inline Semaphore::Semaphore()
{
	sem_init(&sem, 0, 0);
}

inline Semaphore::~Semaphore()
{
	sem_destroy(&sem);
}

inline void Semaphore::down()
{
	sem_wait(&sem);
}

inline void Semaphore::up()
{
	sem_post(&sem);
}

