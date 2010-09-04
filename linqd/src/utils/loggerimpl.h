#ifndef _LOGGER_IMPL_H
#define _LOGGER_IMPL_H

#include "logger.h"
#include "tcpstream.h"


class LoggerImpl : public Logger, public TCPStream {
public:
	LoggerImpl();

	virtual void log(int level, const char *fmt, ...);

	// Open a connection to logd
	// prefix is the string that appears at the beginning of every log message.
	bool open(uint32 ip, int port, int level, const char *prefix);

private:
	int logLevel;
};


#endif
