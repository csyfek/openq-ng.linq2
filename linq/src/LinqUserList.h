#ifndef __LINQ_USER_LIST_H__
#define __LINQ_USER_LIST_H__

#include <list>
#include <string>

#include "LinqDB.h"

class LinqUserList : public DBSerialize
{
public:
	LinqUserList();
	virtual ~LinqUserList();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	void addUser(const char *user);
	void getAllUsers();

	std::list<std::string> userList;
};

#endif
