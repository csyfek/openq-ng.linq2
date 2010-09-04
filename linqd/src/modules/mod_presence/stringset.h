#ifndef _STRING_SET_H
#define _STRING_SET_H

#include "pool.h"


#define HASH_SIZE		101

struct HASH_ITEM {
	HASH_ITEM *next;
};


class StringSet {
public:
	StringSet(Pool *pool);

	unsigned int hashCode(const char *str);
	bool add(const char *str);
	bool remove(const char *str);
	bool contains(const char *str);

	HASH_ITEM *buckets[HASH_SIZE];

private:
	Pool *memPool;
};


inline char *STR(HASH_ITEM *p)
{
	return (char *) (p + 1);
}


#endif
