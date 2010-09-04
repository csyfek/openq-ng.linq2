#include "sessionhash.h"
#include <string.h>

#define HASH_SIZE		9973


SessionHash::SessionHash()
{
	buckets = new ListHead[HASH_SIZE];
}

SessionHash::~SessionHash()
{
	delete []buckets;
}

inline ClientSession *SessionHash::get(ListHead *list, const char *name)
{
	ListHead *pos;

	LIST_FOR_EACH(pos, list) {
		ClientSession *s = LIST_ENTRY(pos, ClientSession, hashItem);
		if (strcmp(s->userName, name) == 0)
			return s;
	}
	return NULL;
}

ClientSession *SessionHash::get(const char *name)
{
	int i = (hashCode(name) % HASH_SIZE);

	mutex.lock();
	ClientSession *s = get(buckets + i, name);
	if (s)
		s->addRef();
	mutex.unlock();

	return s;
}

ClientSession *SessionHash::remove(const char *name)
{
	int i = (hashCode(name) % HASH_SIZE);

	mutex.lock();

	ClientSession *s = get(buckets + i, name);
	if (s)
		s->hashItem.remove();

	mutex.unlock();

	return s;
}

void SessionHash::put(ClientSession *s)
{
	int i = (hashCode(s->userName) % HASH_SIZE);

	mutex.lock();
	buckets[i].addHead(&s->hashItem);
	mutex.unlock();
}

void SessionHash::walk(HASH_WALKER walker, void *data)
{
	for (int i = 0; i < HASH_SIZE; i++) {
		ListHead *head = buckets + i;
		ListHead *pos;

		LIST_FOR_EACH(pos, head) {
			ClientSession *s = LIST_ENTRY(pos, ClientSession, hashItem);
			walker(s, data);
		}
	}
}
