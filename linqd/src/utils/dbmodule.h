#ifndef _DB_MODULE_H
#define _DB_MODULE_H

#include "dbconn.h"
#include "dllmodule.h"

// db module entry point, get a new db connection.
typedef DBConn *(*GETDBCONN)();


class DBModule {
public:
	DBModule();

	bool load(const char *name);

	// Returns a new db connection
	DBConn *getConn();

private:
	DllModule module;
	GETDBCONN getDBConn;
};


#endif
