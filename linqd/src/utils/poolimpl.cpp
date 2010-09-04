#include "poolimpl.h"
#include "debug.h"
#include <malloc.h>
#include <string.h>


// TODO: Should it be put into a c++ template as a parameter?
#define HEAP_SIZE		64


PoolImpl::PoolImpl()
{
	heapList = NULL;
	bytesLeft = 0;
}

PoolImpl::~PoolImpl()
{
	// Free up all of the heaps
	while (heapList) {
		HEAP *next = heapList->next;
		::free(heapList);
		heapList = next;
	}
}

void *PoolImpl::alloc(int size)
{
	// Buffer overflow. create a new heap
	// TODO: The current heap may be not full
	if (bytesLeft < size) {
		// If size is too large, let it occupy a whole new heap
		int n = (size > HEAP_SIZE ? size : HEAP_SIZE);
		HEAP *p = (HEAP *) malloc(sizeof(HEAP) + n);

		p->size = bytesLeft = n;

		// Add it to the head of the list
		p->next = heapList;
		heapList = p;
	}

	ICQ_ASSERT(heapList != NULL);

	// Allocate memory in the current heap
	void *ptr = (char *) (heapList + 1) + heapList->size - bytesLeft;
	bytesLeft -= size;
	return ptr;
}

// Should it be here?
void PoolImpl::free(void *ptr)
{
	ICQ_ASSERT(false);
}

// Resemble strdup in ANSI C
// TODO: Should it be as an iterface function?
char *PoolImpl::strdup(const char *str)
{
	if (!str)
		return NULL;

	int n = strlen(str) + 1;
	char *p = (char *) alloc(n);
	memcpy(p, str, n);
	return p;
}
