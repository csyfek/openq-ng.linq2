#include "LinqUserList.h"

LinqUserList::LinqUserList()
{
}

LinqUserList::~LinqUserList()
{
}

void LinqUserList::load(DBInStream &in)
{
	int n;
	in >> n;
	userList.clear();
	for (int i = 0; i < n; i++) {
		std::string str;
		in >> str;
		userList.push_back(str);
	}
}

void LinqUserList::save(DBOutStream &out)
{
	out << userList.size();
	std::list<std::string>::iterator iter;
	for (iter = userList.begin(); iter != userList.end(); ++iter) {
		out << *iter;
	}
}

void LinqUserList::addUser(const char *user)
{
	LinqDB::loadUserList(*this);
	std::list<std::string>::iterator iter;
	for (iter = userList.begin(); iter != userList.end(); ++iter) {
		if (*iter == user) {
			userList.erase(iter);
			break;
		}
	}	
	userList.push_front(user);
	LinqDB::saveUserList(*this);
}

void LinqUserList::getAllUsers()
{
	LinqDB::loadUserList(*this);
}
