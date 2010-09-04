#include "LinqMsg.h"

LinqMsg::LinqMsg()
{
}

LinqMsg::LinqMsg(ICQ_MSG &msg)
{
	this->type = msg.type;
	this->from = msg.from;
	this->when = msg.when;
	this->text = msg.text;
}

LinqMsg::~LinqMsg()
{
}

void LinqMsg::load(DBInStream &in)
{
	in >> type >> from >> (uint32)when >> text;
}

void LinqMsg::save(DBOutStream &out)
{
	out << type << from << (uint32)when << text;
}

bool LinqMsg::isSysMsg()
{
	return (type != MSG_TEXT);
	/*return (type == MSG_AUTH_ACCEPTED ||
			type == MSG_AUTH_REQUEST  ||
			type == MSG_AUTH_REJECTED ||
			type == MSG_ADDED ||
			type == MSG_ANNOUNCE);*/
}
