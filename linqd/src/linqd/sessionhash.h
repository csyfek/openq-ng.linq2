#ifndef _SESSION_HASH_H
#define _SESSION_HASH_H

#include "clientsession.h"
#include "list.h"
#include "sync.h"


typedef void (*HASH_WALKER)(ClientSession *s, void *data);


/*
 * Thread-safe session hash
 */
class SessionHash {
public:
	SessionHash();
	~SessionHash();

	ClientSession *get(const char *name);
	ClientSession *remove(const char *name);
	void put(ClientSession *s);
	void walk(HASH_WALKER walker, void *data);

private:
	unsigned int hashCode(const char *key);
	ClientSession *get(ListHead *list, const char *name);

	ListHead *buckets;
	Mutex mutex;
};


inline unsigned int SessionHash::hashCode(const char *p)
{
	unsigned int h = 0;
	while (*p)
		h += (h << 5) + *p++;
	return h;
}


#endif
