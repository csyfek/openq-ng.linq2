#ifndef _PID_FILE_H
#define _PID_FILE_H

#include <string>


class PIDFile {
public:
	PIDFile(const char *name);
	~PIDFile();

	pid_t readPID();
	void writePID();

private:
	std::string fileName;
};


#endif

