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

#include "textstream.h"

// Separator between two fields
const char TEXT_STREAM_SEP = (const char) 0xff;


TextOutStream &TextOutStream::operator <<(uint32 dw)
{
	char tmp[11];
	sprintf(tmp, "%lu", dw);
	return (*this << tmp);
}

TextOutStream &TextOutStream::operator <<(const char *str)
{
	if (cursor > text)
		*(cursor - 1) = TEXT_STREAM_SEP;

	int n = strlen(str) + 1;
	if (cursor + n <= text + sizeof(text)) {
		memcpy(cursor, str, n);
		cursor += n;
	}
	return (*this);
}

TextInStream::TextInStream(const char *s)
{
	strncpy(text, s, sizeof(text));
	cursor = text;
}

TextInStream &TextInStream::operator >>(uint8 &b)
{
	uint32 dw;
	operator >>(dw);
	b = dw;
	return (*this);
}

TextInStream &TextInStream::operator >>(uint16 &w)
{
	uint32 dw;
	operator >>(dw);
	w = dw;
	return (*this);
}

TextInStream &TextInStream::operator >>(uint32 &dw)
{
	string s;
	*this >> s;
	dw = atol(s.c_str());
	return (*this);
}

TextInStream &TextInStream::operator >>(string &s)
{	
	char *p = cursor;

	if (*p == '\0')
		s = "";
	else {
		while (*p != TEXT_STREAM_SEP && *p)
			p++;

		s = string(cursor, p - cursor);
		cursor = p + 1;
	}
	return (*this);
}
