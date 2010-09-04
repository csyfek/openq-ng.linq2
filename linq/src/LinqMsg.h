#ifndef __LINQ_MSG_H__
#define __LINQ_MSG_H__

#include "LinqDB.h"
#include <linq/sessionlistener.h>

class LinqMsg : public DBSerialize{
public:
	LinqMsg();
	LinqMsg(ICQ_MSG &msg);
	virtual ~LinqMsg();

	void save(DBOutStream &out);
	void load(DBInStream &in);

	bool isSysMsg();

	uint8 type;
	std::string from;
	time_t when;
	std::string text;
};

#endif
