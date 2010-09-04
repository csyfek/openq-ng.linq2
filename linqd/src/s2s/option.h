#ifndef _OPTION_H
#define _OPTION_H

#include "types.h"
#include <string>


class Option {
public:
	void load(int argc, char *argv[]);

	uint32 s2s_ip;
	int s2s_port;
	std::string domain;

	uint32 sm_ip;
	int sm_port;

	uint32 resolver_ip;
	int resolver_port;
};


#endif
