#ifndef _LOGGER_H
#define _LOGGER_H


// Log levels...
enum {
	LOG_NONE,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFORMATION,
	LOG_DEBUG
};


class Logger {
public:
	virtual void log(int level, const char *fmt, ...) = 0;
};


#endif
