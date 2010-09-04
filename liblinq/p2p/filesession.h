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

#ifndef _FILE_SESSION_H
#define _FILE_SESSION_H

#include "tcpsessionbase.h"
#include "packet.h"
#include <stdio.h>


class FileSession : public TCPSessionListener {
public:
	FileSession(TCPSessionBase *tcp);
	virtual ~FileSession();

	virtual void destroy() { delete this; }

	virtual void onReceive(uint16 cmd, const char *data, int n);
	virtual void onSend();
	virtual void onClose();

protected:
	virtual const char *getPathName(const char *name, uint32 size) = 0;
	virtual void onFileStart() {}
	virtual void onFileProgress(int n) {}
	virtual void onFileComplete() {}

	bool sendFileInfo(const char *path);

	void onFileInfo(InPacket &in);
	void onFileReceive(InPacket &in);
	void onFileData(InPacket &in);

	TCPSessionBase *tcpSession;
	FILE *file;
	uint32 fileSize;
};


#endif
