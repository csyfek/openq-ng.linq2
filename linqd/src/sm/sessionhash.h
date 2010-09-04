#ifndef _SESSION_HASH_H
#define _SESSION_HASH_H

#include "list.h"
#include "session.h"


class SessionHash {
public:
	SessionHash();
	~SessionHash();

	Session *get(const char *name);
	void put(Session *s);
	int random(Session *sessions[], int n);

private:
	unsigned int hashCode(const char *key);
	Session *get(ListHead *list, const char *name);

	ListHead *buckets;
};


inline unsigned int SessionHash::hashCode(const char *p)
{
	unsigned int h = 0;
	while (*p)
		h += (h << 5) + *p++;
	return h;
}


#endif
