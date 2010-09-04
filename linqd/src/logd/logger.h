#ifndef _LOGGER_H
#define _LOGGER_H

#include <stdio.h>
#include <string>


class Logger {
public:
	Logger();
	~Logger();

	void open(const char *file, int level, const char *prefix);
	void close();
	void log(int level, const char *text);

private:
	FILE *logFile;
	int logLevel;
	std::string logPrefix;
};


#endif
