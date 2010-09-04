#include "stringset.h"
#include <string.h>


StringSet::StringSet(Pool *pool)
{
	memPool = pool;
	
	memset(buckets, 0, sizeof(buckets));
}

unsigned int StringSet::hashCode(const char *p)
{
	unsigned int h = 0;
	while (*p)
		h += (h << 5) + *p++;
	return h;
}

bool StringSet::add(const char *str)
{
	if (contains(str))
		return false;

	int i = (hashCode(str) % HASH_SIZE);
	int len = strlen(str) + 1;

	HASH_ITEM *p = (HASH_ITEM *) memPool->alloc(sizeof(HASH_ITEM) + len);
	memcpy(p + 1, str, len);
	p->next = buckets[i];
	buckets[i] = p;

	return true;
}

bool StringSet::remove(const char *str)
{
	int i = (hashCode(str) % HASH_SIZE);

	HASH_ITEM *p = buckets[i];
	if (!p)
		return false;

	if (strcmp(STR(p), str) == 0) {
		buckets[i] = p->next;
		return true;
	}

	while (p->next) {
		if (strcmp(STR(p->next), str) == 0)
			break;

		p = p->next;
	}
	if (!p->next)
		return false;

	p->next = p->next->next;
	return true;
}

bool StringSet::contains(const char *str)
{
	int i = (hashCode(str) % HASH_SIZE);

	for (HASH_ITEM *p = buckets[i]; p; p = p->next) {
		if (strcmp(STR(p), str) == 0)
			return true;
	}
	return false;
}
