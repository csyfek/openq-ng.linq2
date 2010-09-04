#include "serverhash.h"


#define HASH_SIZE		127


ServerHash::ServerHash()
{
	buckets = new ListHead[HASH_SIZE];
}

ServerHash::~ServerHash()
{
	delete []buckets;
}

Server *ServerHash::get(const char *name)
{
	int i = hashCode(name) % HASH_SIZE;
	ListHead *pos;
	ListHead *head = buckets + i;

	LIST_FOR_EACH(pos, head) {
		Server *s = LIST_ENTRY(pos, Server, hashItem);
		if (strcmp(s->serverName, name) == 0)
			return s;
	}
	return NULL;
}

void ServerHash::put(Server *s)
{
	int i = hashCode(s->serverName) % HASH_SIZE;
	buckets[i].addHead(&s->hashItem);
}
