#include "logger.h"
#include "debug.h"
#include <time.h>


enum {
	LOG_NONE,
	LOG_ERROR,
	LOG_WARNING,
	LOG_INFORMATION,
	LOG_DEBUG
};


static const char *level2str(int level)
{
	switch (level) {
	case LOG_ERROR:
		return "ERROR";
	case LOG_WARNING:
		return "WARNING";
	case LOG_INFORMATION:
		return "INFORMATION";
	case LOG_DEBUG:
		return "DEBUG";
	}
	return "UNKNOWN";
}

static const char *logTime(time_t t)
{
	static char buf[128];
	static time_t last_t;

	if (t != last_t) {
		struct tm *tm = localtime(&t);
		strftime(buf, 127, "%Y/%m/%d %H:%M:%S", tm);
		last_t = t;
	}
	return buf;
}


Logger::Logger()
{
	logFile = NULL;
	logLevel = 0;
}

Logger::~Logger()
{
	close();
}

void Logger::open(const char *file, int level, const char *prefix)
{
	ICQ_ASSERT(logFile == NULL);

	if (file && *file)
		logFile = fopen(file, "a+");

	if (!logFile)
		logFile = stderr;

	logLevel = level;
	if (prefix)
		logPrefix = prefix;
}

void Logger::close()
{
	if (logFile && logFile != stderr)
		fclose(logFile);
}

void Logger::log(int level, const char *text)
{
	ICQ_ASSERT(logFile != NULL);

	if (level > logLevel)
		return;

	fprintf(logFile, "%s| %s %s: %s\n",
		logTime(time(NULL)), logPrefix.c_str(), level2str(level), text);

	fflush(logFile);
}
