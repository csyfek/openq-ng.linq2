#ifndef _OPTION_H
#define _OPTION_H

#include "types.h"
#include <string>

using namespace std;


class Option {
public:
	void load(int argc, char *argv[]);

	// Is registration enabled?
	bool enableReg;
	string adminUser;

	// The listen address for client
	uint32 c2s_ip;
	int c2s_port;

	// The listen address for real server
	uint32 linqd_ip;
	int linqd_port;

	// Logger
	uint32 log_ip;
	int log_port;
	int log_level;

	// db information
	string db_impl;
	string db_host;
	int db_port;
	string db_user;
	string db_passwd;
	string db_name;
};


#endif
