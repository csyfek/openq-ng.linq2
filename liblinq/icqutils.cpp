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

#include <string.h>
#include "icqutils.h"


void parseAccount(const char *account, string &name, string &domain)
{
	char *p = strchr(account, '@');
	if (p != NULL) {
		name = string(account, p - account);
		domain = string(p + 1);
	} else {
		name = account;
		domain = "";
	}
}
