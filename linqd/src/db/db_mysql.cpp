#include "types.h"
#include "dbconn.h"
#include "debug.h"

#ifdef _WIN32
// Must be defined before mysql.h, otherwise mysql will complain
#include <winsock.h>
#endif

#include <mysql.h>


class MySQLResult : public DBResult {
public:
	MySQLResult() { result = NULL; }

	void set(MYSQL_RES *res);

	virtual void destroy();
	virtual DB_ROW fetchRow();
	virtual unsigned long *fetchLengths();

private:
	MYSQL_RES *result;
};


class MySQLConn : public DBConn {
public:
	MySQLConn();
	~MySQLConn();

	virtual bool init(const char *host, const char *user, const char *passwd,
		const char *db, int port);
	virtual void destroy() { delete this; }

	virtual unsigned int escapeString(char *to, const char *from, int len);
	virtual bool query(const char *sql, int n);
	virtual DBResult *getResult();
	virtual unsigned long lastInsertID();
	virtual unsigned long getAffectedRows();

private:
	MYSQL *mysql;

	// So that we need not to new a result every query
	MySQLResult result;
};


void MySQLResult::destroy()
{
	ICQ_ASSERT(result != NULL);

	mysql_free_result(result);
	result = NULL;
}

void MySQLResult::set(MYSQL_RES *res)
{
	ICQ_ASSERT(result == NULL);

	result = res;
}

DB_ROW MySQLResult::fetchRow()
{
	return mysql_fetch_row(result);
}

unsigned long *MySQLResult::fetchLengths()
{
	return mysql_fetch_lengths(result);
}


MySQLConn::MySQLConn()
{
	mysql = NULL;
}

MySQLConn::~MySQLConn()
{
	if (mysql)
		mysql_close(mysql);
}

bool MySQLConn::init(const char *host, const char *user, const char *passwd,
					 const char *db, int port)
{
	mysql = mysql_init(NULL);
	if (!mysql) {
		ICQ_LOG("mysql_init() failed\n");
		return false;
	}

	if (!mysql_real_connect(mysql, host, user, passwd, db, port, NULL, 0)) {
		ICQ_LOG("Can not connect to MySQL server: %s\n", mysql_error(mysql));
		return false;
	}

	return true;
}

unsigned int MySQLConn::escapeString(char *to, const char *from, int len)
{
	return mysql_real_escape_string(mysql, to, from, len);
}

bool MySQLConn::query(const char *sql, int n)
{
	ICQ_LOG("SQL: %.*s\n", n, sql);

	if (mysql_real_query(mysql, sql, n) != 0) {
		ICQ_LOG("mysql_real_query() failed:\n%s\n", mysql_error(mysql));
		return false;
	}

	return true;
}

unsigned long MySQLConn::lastInsertID()
{
	return (unsigned long) mysql_insert_id(mysql);
}

unsigned long MySQLConn::getAffectedRows()
{
	return (unsigned long) mysql_affected_rows(mysql);
}

DBResult *MySQLConn::getResult()
{
	MYSQL_RES *res = mysql_store_result(mysql);
	if (!res)
		return NULL;

	result.set(res);
	return &result;
}


ICQ_EXPORT DBConn *getDBConn()
{
	return new MySQLConn;
}
