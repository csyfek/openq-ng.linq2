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

#ifndef _TEXT_STREAM_H
#define _TEXT_STREAM_H

#include "icqtypes.h"
#include <string>

using namespace std;

#define MAX_TEXT_SIZE	450

/*
 * Since there may be many fields encoded in the text field of a message,
 * this class is an utility to do this.
 */
class TextOutStream {
public:
	TextOutStream() {
		cursor = text;
	}

	TextOutStream &operator <<(uint32 dw);
	TextOutStream &operator <<(const char *str);
	operator const char *() {
		return text;
	}

private:
	char text[MAX_TEXT_SIZE];
	char *cursor;
};

class TextInStream {
public:
	TextInStream(const char *s);
	
	TextInStream &operator >>(uint8 &b);
	TextInStream &operator >>(uint16 &w);
	TextInStream &operator >>(uint32 &dw);
	TextInStream &operator >>(string &s);

private:
	char text[MAX_TEXT_SIZE];
	char *cursor;
};


#endif
