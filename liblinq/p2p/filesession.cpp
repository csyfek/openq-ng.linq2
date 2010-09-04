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

#include "filesession.h"
#include <string.h>

enum {
	FILE_CMD_INFO = 0x1000,
	FILE_CMD_RECEIVE,
	FILE_CMD_DATA,
};

#ifdef _WIN32
#define PATH_SEPARATOR		'\\'
#else
#define PATH_SEPARATOR		'/'
#endif


FileSession::FileSession(TCPSessionBase *tcp)
{
	tcpSession = tcp;

	file = NULL;
	fileSize = 0;
}

FileSession::~FileSession()
{
	if (file)
		fclose(file);
}

bool FileSession::sendFileInfo(const char *path)
{
	file = fopen(path, "rb");
	if (!file)
		return false;

	const char *p = strrchr(path, PATH_SEPARATOR);
	const char *name = (p ? p + 1 : path);

	fseek(file, 0, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	OutPacket out;
	out << name << fileSize;
	tcpSession->sendPacket(FILE_CMD_INFO, out.data, out.getLength());
	return true;
}

void FileSession::onSend()
{
	if (file) {
		char buf[960];
		int n = fread(buf, 1, sizeof(buf), file);

		if (n > 0) {
			OutPacket out;
			out.writeData(buf, n);

			if (tcpSession->sendPacket(FILE_CMD_DATA, out.data, out.getLength()))
				onFileProgress(n);
		}

		if (n < sizeof(buf)) {
			tcpSession->enableWrite(false);

			fclose(file);
			file = NULL;

			onFileComplete();
		}
	}

	if (!file)
		tcpSession->destroy();
}

void FileSession::onClose()
{
	if (file) {
		fclose(file);
		file = NULL;
	}

	onFileComplete();

	tcpSession->destroy();
}

void FileSession::onReceive(uint16 cmd, const char *data, int n)
{
	InPacket in(data, n);

	switch (cmd) {
	case FILE_CMD_INFO:
		onFileInfo(in);
		break;

	case FILE_CMD_RECEIVE:
		onFileReceive(in);
		break;

	case FILE_CMD_DATA:
		onFileData(in);
		break;
	}
}

void FileSession::onFileInfo(InPacket &in)
{
	if (file)
		return;

	const char *name;
	in >> name >> fileSize;

	const char *path = getPathName(name, fileSize);
	if (!path)
		return;

	file = fopen(path, "wb");
	if (file) {
		tcpSession->sendPacket(FILE_CMD_RECEIVE, NULL, 0);
		onFileStart();
	}
}

void FileSession::onFileReceive(InPacket &in)
{
	if (file) {
		tcpSession->enableWrite(true);
		onFileStart();
	}
}

void FileSession::onFileData(InPacket &in)
{
	if (!file)
		return;

	int n;
	const char *data = in.readData(n);

	if (data && fwrite(data, n, 1, file) == 1)
		onFileProgress(n);
}
