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

	uint32 log_ip;
	int log_port;
	int log_level;

	uint32 c2s_ip;
	int c2s_port;

	string db_impl;
	DB_INFO db_info;
};


#endif
