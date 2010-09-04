#include "types.h"
#include "dbconn.h"
#include "debug.h"

#include <windows.h>
#include <sql.h>
#include <sqlext.h>


// The maximum fields that a table can hold, hope this is enough
#define MAX_NUM_FIELDS		128


static SQLHENV henv;


class ODBCResult : public DBResult {
public:
	ODBCResult();
	~ODBCResult();

	void set(HSTMT hstmt);

	virtual void destroy();
	virtual DB_ROW fetchRow();
	virtual unsigned long *fetchLengths() { return lengths; }

private:
	HSTMT hstmt;
	int numFields;
	char *row[MAX_NUM_FIELDS];
	unsigned long lengths[MAX_NUM_FIELDS];
};


class ODBCConn : public DBConn {
public:
	ODBCConn();
	~ODBCConn();

	virtual bool init(const char *host, const char *user, const char *passwd,
		const char *db, int port);
	virtual void destroy() { delete this; }

	virtual unsigned int escapeString(char *to, const char *from, int len);
	virtual bool query(const char *sql, int n);
	virtual DBResult *getResult();
	virtual unsigned long lastInsertID();
	virtual unsigned long getAffectedRows();

private:
	SQLHDBC hdbc;
	HSTMT hstmt;

	ODBCResult result;
};


ODBCConn::ODBCConn()
{
	hdbc = NULL;
	hstmt = NULL;
}

ODBCConn::~ODBCConn()
{
	if (hstmt)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	if (hdbc) {
		SQLDisconnect(hdbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
	}
}

bool ODBCConn::init(const char *host, const char *user, const char *passwd,
					const char *db, int port)
{
	// Allocate a connection handle
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);
	if (ret != SQL_SUCCESS)
		return false;

	// Connect to db server
	ret = SQLConnect(hdbc,
		(SQLCHAR *) db, strlen(db),
		(SQLCHAR *) user, strlen(user),
		(SQLCHAR *) passwd, strlen(passwd));

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO) {
		ICQ_LOG("Can not connect to dsn %s\n", db);
		return false;
	}

	return true;
}

bool ODBCConn::query(const char *sql, int n)
{
	ICQ_LOG("SQL: %.*s\n", n, sql);

	// Free statment handle if previously allocated
	if (hstmt)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt);

	// Allocated a statement handle, does it have performance problem?
	SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, hdbc, &hstmt);
	if (ret != SQL_SUCCESS)
		return false;

	// Thanks, but I want to escape string myself:-)
	SQLSetStmtAttr(hstmt, SQL_ATTR_NOSCAN, (SQLPOINTER) SQL_NOSCAN_ON, 0);

	// Execute sql directly
	ret = SQLExecDirect(hstmt, (SQLCHAR *) sql, n);
	if (ret != SQL_SUCCESS) {
		ICQ_LOG("SQLExecDirect() failed\n");
		return false;
	}
	return true;
}

DBResult *ODBCConn::getResult()
{
	result.set(hstmt);
	return &result;
}

unsigned int ODBCConn::escapeString(char *to, const char *from, int len)
{
	char *p = to;

	// ' --> ''
	// TODO: Any other characters need to be escaped?
	while (len-- > 0) {
		switch (*from) {
		case '\'': *p++ = '\''; break;
		}

		*p++ = *from++;
	}

	return p - to;
}

unsigned long ODBCConn::getAffectedRows()
{
	SQLINTEGER n;
	SQLRowCount(hstmt, &n);
	return n;
}

unsigned long ODBCConn::lastInsertID()
{
	// Sorry, ODBC does not support last insert id
	return 0;
}


ODBCResult::ODBCResult()
{
	hstmt = NULL;
	numFields = 0;

	memset(row, 0, sizeof(row));
	memset(lengths, 0, sizeof(lengths));
}

ODBCResult::~ODBCResult()
{
	ICQ_ASSERT(hstmt == NULL);
}

void ODBCResult::set(HSTMT h)
{
	ICQ_ASSERT(hstmt == NULL);

	hstmt = h;

	SQLSMALLINT n;
	SQLNumResultCols(hstmt, &n);

	numFields = n;

	ICQ_ASSERT(n <= MAX_NUM_FIELDS);
}

void ODBCResult::destroy()
{
	ICQ_ASSERT(hstmt != NULL);

	for (int i = 0; i < numFields; i++) {
		if (row[i]) {
			free(row[i]);
			row[i] = NULL;
		}
	}

	hstmt = NULL;
	numFields = 0;
}

DB_ROW ODBCResult::fetchRow()
{
	SQLRETURN ret = SQLFetch(hstmt);
	if (ret != SQL_SUCCESS)
		return NULL;

	char buf[8192];		// Is it enough?
	SQLINTEGER len;

	for (int i = 0; i < numFields; i++) {
		// Free it if already allocated
		if (row[i])
			free(row[i]);

		ret = SQLGetData(hstmt, i + 1, SQL_C_CHAR, buf, sizeof(buf), &len);
		if (ret != SQL_SUCCESS) {
			ICQ_LOG("SQLGetData() failed\n");

			row[i] = NULL;
			lengths[i] = 0;
			continue;
		}

		// When will len < 0? Sorry, I forget it, but it really has happened.
		if (len < 0) {
			len = 0;
			*buf = '\0';
		}
		lengths[i] = len;
		row[i] = (char *) malloc(len + 1);	// including NULL
		memcpy(row[i], buf, len + 1);
	}
	return row;
}


/*
 * Dll entry point
 */
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpvReserved)
{
	SQLRETURN ret;

	switch (fdwReason) {
	case DLL_PROCESS_ATTACH:
		// Allocate a global environment handle
		ret = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
		if (ret != SQL_SUCCESS)
			return FALSE;

		ret = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION,
			(SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER);

		if (ret != SQL_SUCCESS) {
			ICQ_LOG("SQLSetEnvAttr() failed\n");
			return FALSE;
		}
		return TRUE;

	case DLL_PROCESS_DETACH:
		if (henv)
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
	}

	return TRUE;
}

ICQ_EXPORT DBConn *getDBConn()
{
	return new ODBCConn();
}
