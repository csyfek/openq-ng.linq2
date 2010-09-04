#ifndef _OPTION_H
#define _OPTION_H

#include "types.h"
#include <string>


class Option {
public:
	void load(int argc, char *argv[]);

	bool prefix;
	std::string file;
	uint32 ip;
	int port;
};


#endif
