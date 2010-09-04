#ifndef _SQL_STMT_H
#define _SQL_STMT_H

#include "types.h"
#include "dbconn.h"
#include <string.h>


// Is it enough for binary data? (blob)
#define MAX_SQL_LEN				4096

// Append a raw string to sql statement.
// str MUST be a constant string that is allocated at compile time
// Performance hack 8-)
#define SQL_APPEND(sql, str)	sql.append(str, sizeof(str) - 1)


class SQLStmt {
public:
	SQLStmt(DBConn *conn);

	const char *getData() { return data; }
	int getLength() { return (cursor - data); }
	void reset() { cursor = data; }

	SQLStmt &operator <<(char c);
	SQLStmt &operator <<(unsigned int n);
	SQLStmt &operator <<(ICQ_STR &str);
	SQLStmt &operator <<(const char *str);

	// Append escaped string
	void writeString(const char *str, int len);

	void append(const char *data, int n);

private:
	char data[MAX_SQL_LEN];
	char *cursor;
	DBConn *dbConn;
};


inline SQLStmt::SQLStmt(DBConn *conn)
{
	dbConn = conn;
	reset();
}

inline SQLStmt &SQLStmt::operator <<(char c)
{
	*cursor++ = c;
	return (*this);
}

inline SQLStmt &SQLStmt::operator <<(const char *str)
{
	writeString(str, strlen(str));
	return (*this);
}

inline SQLStmt &SQLStmt::operator <<(ICQ_STR &str)
{
	writeString(str.text, str.len);
	return (*this);
}

inline void SQLStmt::append(const char *data, int n)
{
	if (getLength() + n <= MAX_SQL_LEN) {
		memcpy(cursor, data, n);
		cursor += n;
	}
}


#endif
