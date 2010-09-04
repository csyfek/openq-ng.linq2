#ifndef _SESSION_HASH_H
#define _SESSION_HASH_H

#include "list.h"
#include "session.h"


class SessionHash {
public:
	SessionHash();
	~SessionHash();

	Session *get(uint32 ip, uint16 port);
	void put(Session *s);

private:
	// NOTE: Must return unsigned int, so that when modulus by n,
	// the result will always be positive
	unsigned int hashCode(uint32 ip, uint16 port);

	ListHead *buckets;
};


inline unsigned int SessionHash::hashCode(uint32 ip, uint16 port)
{
	// Stolen from Linux:-)
	unsigned int h = ip ^ port;
	h ^= (h >> 16);
	h ^= (h >> 8);
	return h;
}


#endif
