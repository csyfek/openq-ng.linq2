#ifndef _SESSION_H
#define _SESSION_H

#include "list.h"
#include "packet.h"


#define MAX_NAME_LEN	16
#define MAX_NICK_LEN	16

class Client;


class Session {
public:
	Session(Client *c, const char *name);

	void handleRequest(InPacket &in);

	void onUpdateUserInfo(InPacket &in);
	void onChangeStatus(InPacket &in);

	ListHead hashItem;
	ListHead listItem;
	Client *client;
	char userName[MAX_NAME_LEN + 1];
	uint32 status;

private:
	void appendSearchResult(OutPacket &out);
	void onSearchRandom(InPacket &in);
	void onSearch(InPacket &in);

	char nickName[MAX_NICK_LEN + 1];
	uint8 auth, gender;
	uint32 birth;
};


#endif
