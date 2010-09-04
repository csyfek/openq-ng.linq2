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

#include "udppacket.h"


UDPOutPacket::UDPOutPacket()
{
	expiry = 0;
	attempts = 0;
	cmd = 0;
	seq = 0;
}

UDPInPacket::UDPInPacket(const char *data, int len)
	: InPacket(data, len)
{
	*this >> header.ver >> header.reserved;
	*this >> header.sid >> header.seq >> header.cmd;
}
