#include "LinqContactInfo.h"


LinqContactInfo::LinqContactInfo()
{
	gender = 0;
	birth = 0;
}

LinqContactInfo::LinqContactInfo(CONTACT_INFO &info)
{
	if (info.name)
		name = info.name;
	nick = info.nick;
	gender = info.gender;
	birth = info.birth;
	email = info.email;
	country = info.country;
	city = info.city;
	address = info.address;
	postcode = info.postcode;
	tel = info.tel;
	mobile = info.mobile;
	realname = info.realname;
	occupation = info.occupation;
	homepage = info.homepage;
	intro = info.intro;
}

LinqContactInfo::~LinqContactInfo()
{
}

void LinqContactInfo::load(DBInStream &in)
{
	in >> nick >> gender >> birth >> email >> country >> city;
	in >> address >> postcode >> tel >> mobile;
	in >> realname >> occupation >> homepage >> intro;
}

void LinqContactInfo::save(DBOutStream &out)
{
	out << nick << gender << birth << email << country << city;
	out << address << postcode << tel << mobile;
	out << realname << occupation << homepage << intro;
}


LinqContactLocalInfo::LinqContactLocalInfo()
{
	group = 0;
}

LinqContactLocalInfo::~LinqContactLocalInfo()
{
}

void LinqContactLocalInfo::load(DBInStream &in)
{
	in >> group >> remark;
}

void LinqContactLocalInfo::save(DBOutStream &out)
{
	out << group << remark;
}


LinqUserInfo::LinqUserInfo()
{
	auth = 0;
}

LinqUserInfo::LinqUserInfo(USER_INFO &info) : LinqContactInfo(info)
{
	auth = info.auth;
}

void LinqUserInfo::load(DBInStream &in)
{
	LinqContactInfo::load(in);

	in >> auth;
}

void LinqUserInfo::save(DBOutStream &out)
{
	LinqContactInfo::save(out);
	
	out << auth;
}

int operator==(const LinqUserInfo& left, const LinqUserInfo& right)
{
	return (left.nick == left.nick
			&& left.gender == right.gender
			&& left.birth == right.birth
			&& left.email == right.email
			&& left.country == right.country
			&& left.city == right.city
			&& left.address == right.address
			&& left.postcode == right.postcode
			&& left.tel == right.tel
			&& left.mobile == right.mobile
			&& left.realname == right.realname
			&& left.occupation == right.occupation
			&& left.homepage == right.homepage
			&& left.intro == right.intro
			&& left.auth == right.auth
			);
}
