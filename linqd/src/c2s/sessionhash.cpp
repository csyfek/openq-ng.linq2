#include "sessionhash.h"


// A prime number smaller than 10,000, hope it is enough
#define HASH_SIZE		99991


SessionHash::SessionHash()
{
	buckets = new ListHead[HASH_SIZE];
}

SessionHash::~SessionHash()
{
	delete []buckets;
}

Session *SessionHash::get(uint32 ip, uint16 port)
{
	// Locate the bucket
	int i = (hashCode(ip, port) % HASH_SIZE);

	ListHead *head = buckets + i;
	ListHead *pos;

	// Go through the linked list on this bucket
	LIST_FOR_EACH(pos, head) {
		Session *s = LIST_ENTRY(pos, Session, hashItem);
		if (s->sessionIP == ip && s->sessionPort == port)
			return s;
	}
	return NULL;
}

void SessionHash::put(Session *s)
{
	int i = (hashCode(s->sessionIP, s->sessionPort) % HASH_SIZE);

	buckets[i].add(&s->hashItem);
}
