/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2003 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#include "packet.h"
#include "socket.h"
#include <string.h>


OutPacket::OutPacket()
{
	reset();
}

OutPacket &OutPacket::operator <<(uint8 b)
{
	if (getBytesLeft() >= sizeof(b))
		*cursor++ = b;

	return (*this);
}

OutPacket &OutPacket::operator <<(uint16 w)
{
	if (getBytesLeft() >= sizeof(w)) {
		*(uint16 *) cursor = htons(w);
		cursor += sizeof(w);
	}
	return (*this);
}

OutPacket &OutPacket::operator <<(uint32 dw)
{
	if (getBytesLeft() >= sizeof(dw)) {
		*(uint32 *) cursor = htonl(dw);
		cursor += sizeof(dw);
	}
	return (*this);
}

OutPacket &OutPacket::operator <<(const char *str)
{
	writeData(str, strlen(str) + 1);
	return *this;
}

void OutPacket::writeData(const char *data, int n)
{
	uint16 len = n;

	if (getBytesLeft() - n >= (int) sizeof(len)) {
		operator <<(len);

		memcpy(cursor, data, len);
		cursor += len;
	}
}

InPacket::InPacket(const char *data, int n)
{
	this->data = cursor = data;
	length = n;
}

InPacket &InPacket::operator >>(uint8 &b)
{
	if (getBytesLeft() >= sizeof(b))
		b = *cursor++;
	else
		b = 0;

	return (*this);
}

InPacket &InPacket::operator >>(uint16 &w)
{
	if (getBytesLeft() >= sizeof(w)) {
		w = ntohs(*(uint16 *) cursor);
		cursor += sizeof(w);
	} else
		w = 0;
	return (*this);
}

InPacket &InPacket::operator >>(uint32 &dw)
{
	if (getBytesLeft() >= sizeof(dw)) {
		dw = ntohl(*(uint32 *) cursor);
		cursor += sizeof(dw);
	} else
		dw = 0;
	return (*this);
}

InPacket &InPacket::operator >>(const char *&str)
{
	int n;
	const char *data = readData(n);

	if (n > 0 && !data[n - 1])
		str = data;
	else
		str = "";

	return (*this);
}

InPacket &InPacket::operator >>(string &str)
{
	const char *p;
	*this >> p;
	str = p;
	return (*this);
}

const char *InPacket::readData(int &n)
{
	const char *data = NULL;
	n = 0;

	uint16 len;
	operator >>(len);

	if (len && getBytesLeft() >= len) {
		n = len;
		data = cursor;
		cursor += len;
	}
	return data;
}
