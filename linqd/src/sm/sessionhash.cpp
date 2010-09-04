#include "sessionhash.h"
#include "proto.h"
#include <string.h>
#include <stdlib.h>

#define HASH_SIZE		99991


inline uint32 rand20()
{
	return ((rand() << 12) | (rand() & 0xfff));
}


SessionHash::SessionHash()
{
	buckets = new ListHead[HASH_SIZE];
}

SessionHash::~SessionHash()
{
	delete []buckets;
}

inline Session *SessionHash::get(ListHead *list, const char *name)
{
	ListHead *pos;
	
	LIST_FOR_EACH(pos, list) {
		Session *s = LIST_ENTRY(pos, Session, hashItem);
		if (strcmp(s->userName, name) == 0)
			return s;
	}
	return NULL;
}

Session *SessionHash::get(const char *name)
{
	int i = (hashCode(name) % HASH_SIZE);
	return get(buckets + i, name);
}

void SessionHash::put(Session *s)
{
	int i = (hashCode(s->userName) % HASH_SIZE);
	buckets[i].add(&s->hashItem);
}

int SessionHash::random(Session *sessions[], int n)
{
	int start = rand20() % HASH_SIZE;
	int i = start;
	int count = 0;

	do {
		ListHead *head = buckets + i;
		ListHead *pos;

		LIST_FOR_EACH(pos, head) {
			Session *s = LIST_ENTRY(pos, Session, hashItem);
			if (s->status == STATUS_INVIS)
				continue;

			sessions[count++] = s;
			if (count >= n)
				return count;
		}
		if (++i >= HASH_SIZE)
			i = 0;

	} while (i != start);

	return count;
}
