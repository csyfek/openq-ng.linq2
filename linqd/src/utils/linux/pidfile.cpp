#include "pidfile.h"
#include "debug.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#define PID_DIR		"/var/run/linqd"

PIDFile::PIDFile(const char *name)
{
	fileName = PID_DIR"/";
	fileName += name;
	fileName += ".pid";
}

PIDFile::~PIDFile()
{
	remove(fileName.c_str());
}

pid_t PIDFile::readPID()
{
	pid_t pid = 0;

	FILE *file = fopen(fileName.c_str(), "rt");
	if (file) {
		int i;
		if (fscanf(file, "%d", &i) == 1)
			pid = (pid_t) i;
		fclose(file);
	}
	return pid;
}

void PIDFile::writePID()
{
	FILE *file = fopen(fileName.c_str(), "wt");
	if (!file) {
		ICQ_LOG("Can not open pid file to write: %s\n", fileName.c_str());
		return;
	}
	fprintf(file, "%d", (int) getpid());
	fclose(file);
}
