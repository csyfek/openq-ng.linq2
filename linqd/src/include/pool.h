#ifndef _POOL_H
#define _POOL_H


/*
 * This macro should be put in a class definition
 */
#define DECLARE_POOL		\
public:	\
	void *operator new(size_t size, Pool *p) {	\
		return p->alloc(size);	\
	}	\
	void operator delete(void *ptr, Pool *p) {	\
	}


/*
 * Why we need a pool? It is a long story:-)
 * TODO: free is neccessory? I may remove it someday.
 */
class Pool {
public:
	virtual void *alloc(int size) = 0;
	virtual void free(void *ptr) = 0;
	virtual char *strdup(const char *str) = 0;
};


#endif
