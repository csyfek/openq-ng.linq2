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

#ifndef _PROXY_H
#define _PROXY_H

#include <string>

using namespace std;

enum {
	PROXY_NONE = -1,
	PROXY_SOCKS5,
	PROXY_HTTPS,

	NUM_PROXY_TYPES
};


struct PROXY_INFO {
	PROXY_INFO() {
		type = PROXY_NONE;
		port = 0;
		resolve = false;
	}

	int type;
	string host;
	int port;
	string user;
	string passwd;
	bool resolve;		// Resolve locally
};


#endif
