#ifndef _POOL_IMPL_H
#define _POOL_IMPL_H

#include "pool.h"


// A pool contains many heaps
struct HEAP {
	HEAP *next;
	int size;
};


class PoolImpl : public Pool {
public:
	PoolImpl();
	~PoolImpl();

	virtual void *alloc(int size);
	virtual void free(void *ptr);
	virtual char *strdup(const char *str);

private:
	HEAP *heapList;
	int bytesLeft;	// bytes left in the current heap
};


#endif
