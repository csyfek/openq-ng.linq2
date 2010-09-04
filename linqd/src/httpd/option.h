#ifndef _OPTION_H
#define _OPTION_H

#include "types.h"


class Option {
public:
	void load(int argc, char *argv[]);

	uint32 https_ip;
	int https_port;
	uint32 c2s_ip;
	int c2s_port;
};


#endif
