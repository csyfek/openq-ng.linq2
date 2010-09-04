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

#ifndef _PACKET_H
#define _PACKET_H

#include "icqtypes.h"
#include <time.h>
#include <string>

using namespace std;

#define MAX_PACKET_SIZE		1024


class OutPacket {
public:
	OutPacket();

	void reset() { cursor = data; }
	int getBytesLeft() { return (MAX_PACKET_SIZE - getLength()); }

	OutPacket &operator <<(uint8 b);
	OutPacket &operator <<(uint16 w);
	OutPacket &operator <<(uint32 dw);
	OutPacket &operator <<(const char *str);
	OutPacket &operator <<(const string &str) {
		return (*this << str.c_str());
	}
	void writeData(const char *data, int n);

	const char *getData() { return data; }
	int getLength() { return (cursor - data); }

	char data[MAX_PACKET_SIZE];

private:
	char *cursor;
};


class InPacket {
public:
	InPacket(const char *data, int n);

	InPacket &operator >>(uint8 &b);
	InPacket &operator >>(uint16 &w);
	InPacket &operator >>(uint32 &dw);
	InPacket &operator >>(const char *&str);
	InPacket &operator >>(string &str);
	const char *readData(int &n);

	int getBytesLeft() {
		return (length - (cursor - data));
	}

	const char *cursor;

private:
	const char *data;
	int length;
};


#endif
