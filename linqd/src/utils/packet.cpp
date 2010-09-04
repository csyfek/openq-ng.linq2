#include "packet.h"
#include "debug.h"


void OutPacket::appendString(const char *str, int len)
{
	// LEN + 1 followed by str plus NULL
	uint16 n = len + 1;

	if (n + 2 > getBytesLeft()) {
		ICQ_LOG("Error write string to packet: %s\n", str);
		return;
	}

	*this << n;
	memcpy(cursor, str, n);
	cursor += n;
}


InPacket &InPacket::operator >>(ICQ_STR &str)
{
	uint16 len;
	*this >> len;

	if (len > getBytesLeft() || cursor[len - 1]) {
		ICQ_LOG("Error read string from packet\n");

		str.len = 0;
		str.text = "";
	} else {
		str.len = len - 1;
		str.text = cursor;
		cursor += len;
	}
	return (*this);
}
