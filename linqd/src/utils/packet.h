#ifndef _PACKET_H
#define _PACKET_H

#include "types.h"
#include <string.h>

#ifdef _WIN32
#include <winsock.h>
#else
#include <netinet/in.h>
#endif


// Is this appropriate?
#define MAX_PACKET_SIZE		1024


class OutPacket {
public:
	OutPacket() { reset(); }

	void reset() { cursor = data; }
	int getLength() { return (cursor - data); }
	int getBytesLeft() { return (MAX_PACKET_SIZE - getLength()); }

	OutPacket &operator <<(uint8 b);
	OutPacket &operator <<(uint16 w);
	OutPacket &operator <<(uint32 dw);
	OutPacket &operator <<(ICQ_STR &str);
	OutPacket &operator <<(const char *str);

	// No network order conversion
	void write16(uint16 w);
	void write32(uint32 dw);

	void appendString(const char *str, int n);

	// Append raw data
	void appendData(const char *data, int n);

	char data[MAX_PACKET_SIZE];
	char *cursor;
};


class InPacket {
public:
	InPacket(const char *data, int n);

	void reset() { cursor = data; }
	int getBytesLeft() { return (data + length - cursor); }

	InPacket &operator >>(uint8 &b);
	InPacket &operator >>(uint16 &w);
	InPacket &operator >>(uint32 &dw);
	InPacket &operator >>(ICQ_STR &str);

	// No network order conversion
	uint16 read16();
	uint32 read32();

	const char *cursor;

private:
	const char *data;
	int length;
};


inline OutPacket &OutPacket::operator <<(uint8 b)
{
	if (getBytesLeft() >= sizeof(b))
		*cursor++ = b;
	return (*this);
}

inline OutPacket &OutPacket::operator <<(uint16 w)
{
	write16(htons(w));
	return (*this);
}

inline OutPacket &OutPacket::operator <<(uint32 dw)
{
	write32(htonl(dw));
	return (*this);
}

inline OutPacket &OutPacket::operator <<(ICQ_STR &str)
{
	appendString(str.text, str.len);
	return (*this);
}

inline OutPacket &OutPacket::operator <<(const char *str)
{
	appendString(str, strlen(str));
	return (*this);
}

inline void OutPacket::write16(uint16 w)
{
	if (getBytesLeft() >= sizeof(w)) {
		*(uint16 *) cursor = w;
		cursor += sizeof(w);
	}
}

inline void OutPacket::write32(uint32 dw)
{
	if (getBytesLeft() >= sizeof(dw)) {
		*(uint32 *) cursor = dw;
		cursor += sizeof(dw);
	}
}

inline void OutPacket::appendData(const char *data, int n)
{
	if (getBytesLeft() >= n) {
		memcpy(cursor, data, n);
		cursor += n;
	}
}


inline InPacket::InPacket(const char *d, int n)
{
	data = d;
	length = n;

	reset();
}

inline InPacket &InPacket::operator >>(uint8 &b)
{
	if (getBytesLeft() >= sizeof(b))
		b = *cursor++;
	return (*this);
}

inline InPacket &InPacket::operator >>(uint16 &w)
{
	w = ntohs(read16());
	return (*this);
}

inline InPacket &InPacket::operator >>(uint32 &dw)
{
	dw = ntohl(read32());
	return (*this);
}

inline uint16 InPacket::read16()
{
	uint16 w = 0;
	if (getBytesLeft() >= sizeof(w)) {
		w = *(uint16 *) cursor;
		cursor += sizeof(w);
	}
	return w;
}

inline uint32 InPacket::read32()
{
	uint32 dw = 0;
	if (getBytesLeft() >= sizeof(dw)) {
		dw = *(uint32 *) cursor;
		cursor += sizeof(dw);
	}
	return dw;
}


#endif
