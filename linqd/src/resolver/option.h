#ifndef _OPTION_H
#define _OPTION_H

#include "types.h"


class Option {
public:
	void load(int argc, char *argv[]);

	uint32 ip;
	int port;
};


#endif
