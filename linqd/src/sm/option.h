#ifndef _OPTION_H
#define _OPTION_H

#include "types.h"
#include <string>

using namespace std;


struct DB_INFO {
	string host;
	int port;
	string user;
	string passwd;
	string db;
};


class Option {
public:
	void load(int argc, char *argv[]);

	uint32 sm_ip;
	int sm_port;

	uint32 s2s_ip;
	int s2s_port;

	string db_impl;
	DB_INFO db_info;
};


#endif
