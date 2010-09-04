#ifndef _SERVER_HASH_H
#define _SERVER_HASH_H

#include "server.h"


class ServerHash {
public:
	ServerHash();
	~ServerHash();

	Server *get(const char *name);
	void put(Server *s);

private:
	unsigned int hashCode(const char *p);

	ListHead *buckets;
};


inline unsigned int ServerHash::hashCode(const char *p)
{
	unsigned int h = 0;
	while (*p)
		h += (h << 5) + *p++;
	return h;
}


#endif
