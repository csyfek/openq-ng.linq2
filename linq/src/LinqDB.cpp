/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2002 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#include "LinqDB.h"
#include "LinQ.h"
#include "LinqContactInfo.h"
#include "LinqMsg.h"
#include "LinqOption.h"
#include "LinqGlobalSetting.h"
#include "LinqUserList.h"
#include <db.h>

#include <gtkmm/main.h>

#include <sys/stat.h>
#ifdef __WIN32
#include <dir.h>  // for mkdir()
#endif

//linq.db
#define INDEX_GLOBAL_SETTING	"index_global_setting"
#define INDEX_USER_LIST			"index_user_list"

//user.db
#define INDEX_USER		"#index_user"
#define INDEX_OPTIONS	"#index_options"
#define INDEX_GROUP		"#index_group"

//msg.db
#define INDEX_SYSMSG	"#index_sysmsg"

using namespace std;


DBOutStream &DBOutStream::operator <<(uint8 b)
{
	if (cursor <= data + MAX_BLOCK_SIZE - sizeof(b))
		*cursor++ = b;
	return (*this);
}

DBOutStream &DBOutStream::operator <<(uint16 w)
{
	if (cursor <= data + MAX_BLOCK_SIZE - sizeof(w)) {
		*(uint16 *) cursor = w;
		cursor += sizeof(w);
	}
	return (*this);
}

DBOutStream &DBOutStream::operator <<(uint32 dw)
{
	if (cursor <= data + MAX_BLOCK_SIZE - sizeof(dw)) {
		*(uint32 *) cursor = dw;
		cursor += sizeof(dw);
	}
	return (*this);
}

DBOutStream &DBOutStream::operator <<(int i)
{
	if (cursor <= data + MAX_BLOCK_SIZE - sizeof(i)) {
		*(int *) cursor = i;
		cursor += sizeof(i);
	}
	return (*this);
}

DBOutStream &DBOutStream::operator <<(const char *str)
{
	uint16 len = strlen(str) + 1;
	if (cursor <= data + MAX_BLOCK_SIZE - sizeof(len) - len) {
		*this << len;
		memcpy(cursor, str, len);
		cursor += len;
	}
	return (*this);
}

DBOutStream &DBOutStream::operator <<(std::list<std::string> &strList)
{
	uint16 n = 0;
	char *old = cursor;
	cursor += sizeof(n);
	
	std::list<std::string>::iterator i;
	for (i = strList.begin(); i != strList.end(); i++) {
		operator <<(*i);
		n++;
	}
	char *p = cursor;
	cursor = old;
	operator <<(n);
	cursor = p;
	return (*this);
}

DBInStream &DBInStream::operator >>(uint8 &b)
{
	if (cursor <= data + datalen - sizeof(b))
		b = *cursor++;
	else
		b = 0;
	return (*this);
}

DBInStream &DBInStream::operator >>(uint16 &w)
{
	if (cursor <= data + datalen - sizeof(w)) {
		w = *(uint16 *) cursor;
		cursor += sizeof(w);
	} else
		w = 0;
	return (*this);
}

DBInStream &DBInStream::operator >>(uint32 &dw)
{
	if (cursor <= data + datalen - sizeof(dw)) {
		dw = *(uint32 *) cursor;
		cursor += sizeof(dw);
	} else
		dw = 0;
	return (*this);
}

DBInStream &DBInStream::operator >>(int &i)
{
	if (cursor <= data + datalen - sizeof(i)) {
		i = *(int *) cursor;
		cursor += sizeof(i);
	} else
		i = 0;
	return (*this);
}

DBInStream &DBInStream::operator >>(std::string &str)
{
	uint16 len;
	operator >>(len);

	if (cursor <= data + datalen - len && !cursor[len - 1]) {
		str = cursor;
		cursor += len;
	} else
		str = "";
	return (*this);
}

DBInStream &DBInStream::operator >>(std::list<std::string> &strList)
{
	uint16 num;
	operator >>(num);
	int n = (int) num;

	strList.clear();
	while (n-- > 0) {
		string s;
		operator >>(s);
		strList.push_back(s);
	}
	return (*this);
}


static const char userFile[] = "user.db";
static const char msgFile[] = "msg.db";

static char index_buf[256];

static string dbDir;
static string dbNowUser;


static DB *getDBFullPath(const char *pathName, bool dup)
{
	DB *db;
	if (db_create(&db, NULL, 0) != 0)
		return NULL;

	if (dup && db->set_flags(db, DB_DUP) != 0) {
		db->close(db, 0);
		return NULL;
	}
	if (db->open(db, pathName, NULL, DB_HASH, DB_CREATE, 0600) != 0) {
		db->close(db, 0);
		return NULL;
	}
	return db;
}

static DB *getGlobalDB(bool dup = false)
{
	return getDBFullPath((dbDir + "linq.db").c_str(), dup);
}

static DB *getUserDB(const char *fileName, bool dup = false)
{
	return getDBFullPath((dbDir + dbNowUser + fileName).c_str(), dup);
}

static bool saveBlock(DB *db, char *index, DBSerialize &obj)
{
	DBOutStream out;
	obj.save(out);

	DBC *cursor;
	if (db->cursor(db, NULL, &cursor, 0) != 0) {
		db->close(db, 0);
		return false;
	}

	DBT key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = index;
	key.size = strlen(index);
	data.data = out.getData();
	data.size = out.getSize();

	return (cursor->c_put(cursor, &key, &data, DB_KEYFIRST) == 0);
}

static bool saveBlock(const char *fileName, char *index, DBSerialize &obj, bool dup)
{
	DB *db = getUserDB(fileName, dup);
	if (!db)
		return false;

	int ret = saveBlock(db, index, obj);
	db->close(db, 0);
	return (ret == 0);
}

static bool saveBlock(char *index, DBSerialize &obj, bool dup)
{
	DB *db = getGlobalDB(dup);
	if (!db)
		return false;

	int ret = saveBlock(db, index, obj);
	db->close(db, 0);
	return (ret == 0);
}

static bool loadBlock(DB *db, char *index, DBSerialize &obj)
{
	DBT key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = index;
	key.size = strlen(index);

	if (db->get(db, NULL, &key, &data, 0) != 0) {
		db->close(db, 0);
		return false;
	}

	DBInStream in(data.data, data.size);
	obj.load(in);

	db->close(db, 0);

	return true;
}

static bool loadBlock(const char *fileName, char *index, DBSerialize &obj)
{
	DB *db = getUserDB(fileName, false);
	if (!db)
		return false;

	return loadBlock(db, index, obj);
}

static bool loadBlock(char *index, DBSerialize &obj)
{
	DB *db = getGlobalDB(false);
	if (!db)
		return false;

	return loadBlock(db, index, obj);	
}

static bool delIndex(const char *fileName, char *index)
{
	DB *db = getUserDB(fileName);
	if (!db)
		return false;

	DBT key;
	memset(&key, 0, sizeof(key));
	key.data = index;
	key.size = strlen(index);
	int ret = db->del(db, NULL, &key, 0);

	db->close(db, 0);
	return (ret == 0);
}

void LinqDB::init()
{	
#ifdef __WIN32
	dbDir = linqDataDir;
	dbDir += G_DIR_SEPARATOR_S"usrs"G_DIR_SEPARATOR_S;
#else
	dbDir = g_get_home_dir();
	dbDir += "/.linq/";
#endif

	// initialize the user dir.
	struct stat st;
#ifdef __WIN32
	if (stat (dbDir.c_str(), &st)) {
		mkdir (dbDir.c_str());
	}
#else
	if (stat (dbDir.c_str(), &st))
		mkdir (dbDir.c_str(),00755);
#endif
}

void LinqDB::setNowUser(const char *user, bool initDir)
{
	dbNowUser = Glib::locale_from_utf8(user);
	dbNowUser += G_DIR_SEPARATOR_S;
	
	if (initDir) {
		struct stat st;
		std::string str;
		str = dbDir + dbNowUser;
		
		if (stat (str.c_str(), &st))
#ifdef __WIN32
			mkdir (str.c_str());
#else
			mkdir (str.c_str(), 00755);
#endif
	}
}

bool LinqDB::saveUserList(LinqUserList &userList)
{
	return saveBlock(INDEX_USER_LIST, userList, false);
}

bool LinqDB::loadUserList(LinqUserList &userList)
{
	return loadBlock(INDEX_USER_LIST, userList);
}

bool LinqDB::saveGlobalSetting(LinqGlobalSetting &setting)
{
	return saveBlock(INDEX_GLOBAL_SETTING, setting, false);
}

bool LinqDB::loadGlobalSetting(LinqGlobalSetting &setting)
{
	return loadBlock(INDEX_GLOBAL_SETTING, setting);
}

bool LinqDB::saveMsg(LinqMsg &msg)
{
	if (msg.isSysMsg()) {
		return saveBlock(msgFile, INDEX_SYSMSG, msg, true);
	}
	else {
		strcpy(index_buf, msg.from.c_str());
		return saveBlock(msgFile, index_buf, msg, true);
	}
}

bool LinqDB::delMsg(const char *name)
{
	if (name) {
		strcpy(index_buf, name);
		return delIndex(msgFile, index_buf);
	}
	else
		return delIndex(msgFile, INDEX_SYSMSG);
}

bool LinqDB::delMsg(const char *name, int item)
{
	DB *db = getUserDB(msgFile);
	if (!db)
		return false;

	DBC *cursor;
	if (db->cursor(db, NULL, &cursor, 0) != 0) {
		db->close(db, 0);
		return false;
	}

	char *index;
	if (name) {
		strcpy(index_buf, name);
		index = index_buf;
	}
	else {
		index = INDEX_SYSMSG;
	}

	DBT key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = index;
	key.size = strlen(index);
	if (cursor->c_get(cursor, &key, &data, DB_SET) != 0) {
		db->close(db, 0);
		return false;
	}

	int i;
	for (i = 0; i < item; ++i) {
		if (cursor->c_get(cursor, &key, &data, DB_NEXT_DUP) != 0)
			break;
	}
	if (i < item) {
		cursor->c_close(cursor);
		db->close(db, 0);
		return false;
	}

	int ret = cursor->c_del(cursor, 0);

	cursor->c_close(cursor);
	db->close(db, 0);
	return (ret == 0);
}

bool LinqDB::loadMsg(const char *name, std::list<LinqMsg *> &msgList, uint32 n)
{
	DB *db = getUserDB(msgFile, true);
	if (!db)
		return false;

	DBC *cursor;
	if (db->cursor(db, NULL, &cursor, 0) != 0) {
		db->close(db, 0);
		return false;
	}

	char *index;
	if (name) {
		strcpy(index_buf, name);
		index = index_buf;
	}
	else {
		index = INDEX_SYSMSG;
	}

	DBT key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));
	key.data = index;
	key.size = strlen(index);

	if (cursor->c_get(cursor, &key, &data, DB_SET) != 0) {
		cursor->c_close(cursor);
		db->close(db, 0);
		return false;
	}

	do {
		DBInStream in(data.data, data.size);
		LinqMsg *msg = new LinqMsg;
		msg->load(in);
		msgList.push_front(msg);

	} while (cursor->c_get(cursor, &key, &data, DB_NEXT_DUP) == 0 && (--n > 0));

	cursor->c_close(cursor);
	db->close(db, 0);
	return true;
}

bool LinqDB::saveContactInfo(LinqContactInfo &c)
{
	sprintf(index_buf, "!%s", c.name.c_str());
	return saveBlock(userFile, index_buf, c, false);
}

bool LinqDB::loadContactInfo(LinqContactInfo &c)
{
	sprintf(index_buf, "!%s", c.name.c_str());
	return loadBlock(userFile, index_buf, c);
}

bool LinqDB::saveContactLocalInfo(LinqContactLocalInfo &c)
{
	sprintf(index_buf, "&%s", c.name.c_str());
	return saveBlock(userFile, index_buf, c, false);
}

bool LinqDB::loadContactLocalInfo(LinqContactLocalInfo &c)
{
	sprintf(index_buf, "&%s", c.name.c_str());
	return loadBlock(userFile, index_buf, c);
}

bool LinqDB::loadContactList(std::list<std::string *> &contactList)
{
	DB *db = getUserDB(userFile);
	if (!db)
		return false;

	DBC *cursor;
	if (db->cursor(db, NULL, &cursor, 0) != 0) {
		db->close(db, 0);
		return false;
	}

	DBT key, data;
	memset(&key, 0, sizeof(key));
	memset(&data, 0, sizeof(data));

	while (cursor->c_get(cursor, &key, &data, DB_NEXT) == 0) {
		if (((const char *)(key.data))[0] == '!') {
			string *str = new string((const char *)(key.data) +1, key.size -1);
			contactList.push_back(str);
		}
	}

	cursor->c_close(cursor);
	db->close(db, 0);
	return true;
}

bool LinqDB::delContact(std::string &name)
{
	sprintf(index_buf, "&%s", name.c_str());
	delIndex(userFile, index_buf);
	sprintf(index_buf, "!%s", name.c_str());
	return delIndex(userFile, index_buf);
}

bool LinqDB::saveUserInfo(LinqUserInfo &user)
{
	return saveBlock(userFile, INDEX_USER, user, false);
}

bool LinqDB::loadUserInfo(LinqUserInfo &user)
{
	return loadBlock(userFile, INDEX_USER, user);
}

bool LinqDB::saveOptions(LinqOption &options)
{
	return saveBlock(userFile, INDEX_OPTIONS, options, false);
}

bool LinqDB::loadOptions(LinqOption &options)
{
	return loadBlock(userFile, INDEX_OPTIONS, options);
}

bool LinqDB::saveGroupInfo(DBSerialize &obj)
{
	return saveBlock(userFile, INDEX_GROUP, obj, false);
}

bool LinqDB::loadGroupInfo(DBSerialize &obj)
{
	return loadBlock(userFile, INDEX_GROUP, obj);
}
