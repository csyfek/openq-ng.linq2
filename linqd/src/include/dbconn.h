#ifndef _DB_CONN_H
#define _DB_CONN_H


// char *DB_ROW[]
typedef char **DB_ROW;


class DBResult {
public:
	virtual void destroy() = 0;
	// Fetch a row from the result set
	virtual DB_ROW fetchRow() = 0;
	// Fetch the lengths array of the current row
	virtual unsigned long *fetchLengths() = 0;
};


/*
 * A connection to db server
 */
class DBConn {
public:
	// Connect to db server
	virtual bool init(const char *host, const char *user, const char *passwd,
		const char *db, int port) = 0;
	virtual void destroy() = 0;

	// Escape characters (single quotes for example)
	virtual unsigned int escapeString(char *to, const char *from, int len) = 0;
	// Execute a sql statement
	virtual bool query(const char *sql, int n) = 0;
	// Get the result set of the previously executed sql
	virtual DBResult *getResult() = 0;

	// Returns the last insert id of the auto_increment column.
	// Returns 0 if db does not support it
	virtual unsigned long lastInsertID() = 0;
	// The number of rows affected by the lastest sql command ('UPDATE' eg.)
	virtual unsigned long getAffectedRows() = 0;
};


#endif
