#include "sqlstmt.h"
#include "debug.h"


/*
 * Convert from int to string, just like itoa.
 * bufEnd is the end of the result string buffer.
 * Returns a pointer to the beginning of the string that is NOT NULL-terminated
 */
inline char *conv10(unsigned int num, char *bufEnd)
{
	do {
		*--bufEnd = num % 10 + '0';
		num /= 10;
	} while (num);

	return bufEnd;
}


SQLStmt &SQLStmt::operator <<(unsigned int n)
{
	char buf[16];
	char *p = conv10(n, buf + sizeof(buf));
	append(p, buf + sizeof(buf) - p);

	return (*this);
}

void SQLStmt::writeString(const char *str, int len)
{
	if (getLength() + (len << 1) >= MAX_SQL_LEN - 2) {
		ICQ_LOG("SQLStmt::writeString error\n");
		return;
	}

	// Surrounding by quotes
	*this << '\'';
	cursor += dbConn->escapeString(cursor, str, len);
	*this << '\'';
}
