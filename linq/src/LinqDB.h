#ifndef __LINQ_DB_H__
#define __LINQ_DB_H__

#include <linq/icqtypes.h>

#include <string>
#include <list>

#define MAX_BLOCK_SIZE	4096

/*
 * Utility class that reads from or writes to a data block
 */
class DBOutStream {
public:
	DBOutStream() {
		cursor = data;
	}
	char *getData() {
		return data;
	}
	int getSize() {
		return (cursor - data);
	}

	DBOutStream &operator <<(uint8 b);
	DBOutStream &operator <<(uint16 w);
	DBOutStream &operator <<(uint32 dw);
	DBOutStream &operator <<(int i);
	DBOutStream &operator <<(const char *str);
	DBOutStream &operator <<(std::list<std::string> &strList);
	DBOutStream &operator <<(const std::string &str) {
		return operator <<(str.c_str());
	}

private:
	char data[MAX_BLOCK_SIZE];
	char *cursor;
};

class DBInStream {
public:
	DBInStream(void *d, int n) {
		cursor = data = (char *) d;
		datalen = n;
	}
	DBInStream &operator >>(uint8 &b);
	DBInStream &operator >>(uint16 &w);
	DBInStream &operator >>(uint32 &dw);
	DBInStream &operator >>(int &i);
	DBInStream &operator >>(std::string &str);
	DBInStream &operator >>(std::list<std::string> &strList);

private:
	char *data;
	char *cursor;
	int datalen;
};

class DBSerialize {
public:
	virtual void save(DBOutStream &out) = 0;
	virtual void load(DBInStream &in) = 0;
};

class LinqOption;
class LinqContactInfo;
class LinqContactLocalInfo;
class LinqUserInfo;
class LinqMsg;
class LinqGlobalSetting;
class LinqUserList;

/*
 * Do all the things related with the database.
 */
class LinqDB {
public:
	static void init();
	static void setNowUser(const char *user, bool initDir = false);

	static bool saveUserList(LinqUserList &userList);
	static bool loadUserList(LinqUserList &userList);
	static bool saveGlobalSetting(LinqGlobalSetting &setting);
	static bool loadGlobalSetting(LinqGlobalSetting &setting);

	static bool saveMsg(LinqMsg &msg);
	static bool loadMsg(const char *name, std::list<LinqMsg *> &msgList, uint32 n = -1);
	static bool delMsg(const char *name);
	static bool delMsg(const char *name, int item);
	static bool saveContactInfo(LinqContactInfo &c);
	static bool loadContactInfo(LinqContactInfo &c);	
	static bool saveContactLocalInfo(LinqContactLocalInfo &c);
	static bool loadContactLocalInfo(LinqContactLocalInfo &c);
	static bool loadContactList(std::list<std::string *> &contactList);
	static bool delContact(std::string &name);
	static bool saveUserInfo(LinqUserInfo &user);
	static bool loadUserInfo(LinqUserInfo &user);
	static bool saveOptions(LinqOption &options);
	static bool loadOptions(LinqOption &options);
	static bool saveGroupInfo(DBSerialize &obj);
	static bool loadGroupInfo(DBSerialize &obj);
};

#endif
