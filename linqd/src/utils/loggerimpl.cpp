#include "loggerimpl.h"
#include "debug.h"
#include <stdio.h>
#include <stdarg.h>

#ifdef _WIN32
#define vsnprintf	_vsnprintf
#endif


LoggerImpl::LoggerImpl()
{
	logLevel = 0;
}

bool LoggerImpl::open(uint32 ip, int port, int level, const char *prefix)
{
	logLevel = level;

	OutPacket out;
	out << (uint8) logLevel << prefix;
	sendPacket(out, NULL, 0);

	if (level > 0)
		return connect(ip, port, 30);
	return true;
}

void LoggerImpl::log(int level, const char *fmt, ...)
{
	// Strip it off, should it be in a macro?
	if (level > logLevel)
		return;

	va_list args;
	va_start(args, fmt);

	char buf[4096];
	int n = vsnprintf(buf, sizeof(buf), fmt, args);

	va_end(args);

	ICQ_LOG("%s\n", buf);
	ICQ_STR text = { n, buf };

	OutPacket out;
	out << (uint8) level << text;
	sendPacket(out, NULL, 0);
}
