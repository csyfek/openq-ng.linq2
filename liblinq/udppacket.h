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

#ifndef _UDP_PACKET_H
#define _UDP_PACKET_H

#include "packet.h"

#pragma pack(1)
struct UDP_PACKET_HDR {
	uint16 ver;
	uint32 reserved;
	uint32 sid;
	uint16 seq;
	uint16 cmd;
};
#pragma pack()


class UDPOutPacket : public OutPacket {
public:
	UDPOutPacket();

	time_t expiry;
	int attempts;
	uint32 cmd;
	uint16 seq;
};


class UDPInPacket : public InPacket {
public:
	UDPInPacket(const char *data, int len);

	UDP_PACKET_HDR header;
};


#endif
