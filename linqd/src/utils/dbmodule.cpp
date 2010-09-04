#include "dbmodule.h"
#include "debug.h"


DBModule::DBModule()
{
	getDBConn = NULL;
}

bool DBModule::load(const char *name)
{
	// Get the entry point function
	if (module.load(name))
		getDBConn = (GETDBCONN) module.getSymbol("getDBConn");

	return (getDBConn != NULL);
}

DBConn *DBModule::getConn()
{
	ICQ_ASSERT(getDBConn != NULL);

	return getDBConn();
}
