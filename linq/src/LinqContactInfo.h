#ifndef __LINQ_CONTACT_INFO_H__
#define __LINQ_CONTACT_INFO_H__

#include "LinqDB.h"
#include <linq/sessionlistener.h>

class LinqContactInfo : public DBSerialize{
public:
	LinqContactInfo();
	LinqContactInfo(CONTACT_INFO &info);	
	virtual ~LinqContactInfo();

	void save(DBOutStream &out);
	void load(DBInStream &in);

	std::string name;
	std::string nick;
	uint8 gender;
	uint32 birth;
	std::string email;
	std::string country;
	std::string city;
	std::string address;
	std::string postcode;
	std::string tel;
	std::string mobile;
	std::string realname;
	std::string occupation;
	std::string homepage;
	std::string intro;
};

class LinqContactLocalInfo : public DBSerialize {
public:
	LinqContactLocalInfo();
	virtual ~LinqContactLocalInfo();

	void load(DBInStream &in);
	void save(DBOutStream &out);

	std::string name;
	int group; // -1 means stranger, -2 means ignore, 0 means "my friends"...
	std::string remark;
};

class LinqUserInfo : public LinqContactInfo {
public:
	LinqUserInfo();
	LinqUserInfo(USER_INFO &info);

	void load(DBInStream &in);
	void save(DBOutStream &out);
	
	friend int operator==(const LinqUserInfo& left, const LinqUserInfo& right);

	uint8 auth;
};

#endif
