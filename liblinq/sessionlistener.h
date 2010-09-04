/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright            : (C) 2003 by Zhang Yong                         *
 *   email                : z-yong163@163.com                              *
 ***************************************************************************/

#ifndef _SESSION_LISTENER_H
#define _SESSION_LISTENER_H

#include "icqtypes.h"
#include <time.h>
#include <string>

using namespace std;

// Login reply code
enum {
	LOGIN_SUCCESS,
	LOGIN_INVALID_USER,
	LOGIN_WRONG_PASSWD,
	LOGIN_ERROR_UNKNOWN,
};

// Registration reply code
enum {
	REG_SUCCESS,
	REG_USER_EXISTS,
	REG_ERROR_UNKNOWN,
};

enum {
	AUTH_ACCEPTED,
	AUTH_REQUEST,
	AUTH_REJECTED,
};

// Status
enum {
	STATUS_ONLINE,
	STATUS_OFFLINE,
	STATUS_AWAY,
	STATUS_INVIS
};

// Message types
enum {
	MSG_TEXT,
	MSG_AUTH_ACCEPTED,
	MSG_AUTH_REQUEST,
	MSG_AUTH_REJECTED,
	MSG_ADDED,
	MSG_ANNOUNCE,

	MSG_TCP_REQUEST = 0x80,
	MSG_TCP_ACCEPT,
};

// Online information of a contact
struct ONLINE_INFO {
	string name;
	uint32 status;
	uint16 tcpver;
	uint32 ip;
	uint32 realip;
	uint16 msgport;
};

struct SEARCH_RESULT {
	string name;
	uint32 status;
	uint8 auth;
	string nick;
	uint8 gender;
	uint8 age;
};

#define MF_RELAY	0x01
#define MF_FROM		0x02

#define IS_SYSMSG(msg)	\
	((msg).type != MSG_TEXT)

struct ICQ_MSG {
	uint8 type;
	string contact;
	time_t when;
	string text;
	int flags;
};

// All details about a contact
struct CONTACT_INFO {
	string name;
	string nick;
	uint8 gender;
	uint32 birth;
	string email;
	string country;
	string city;
	string address;
	string postcode;
	string tel;
	string mobile;
	string realname;
	string occupation;
	string homepage;
	string intro;
};

// All details about a user
struct USER_INFO : public CONTACT_INFO {
	uint8 auth;
};


class SessionListener {
public:
	virtual void onAck(uint32 seq) {}
	virtual void onSendError(uint16 seq) {}
	virtual void onConnect(bool connected) {}

	virtual void onRegisterReply(uint8 error, const char *name) {}
	virtual void onLoginReply(uint8 error, const char *domain) {}
	virtual void onAddContactReply(const char *name, uint8 auth) {}
	virtual void onContactListReply(const char *contacts[], int n) {}

	virtual void onUserOnline(ONLINE_INFO &info) {}
	virtual void onUserOffline(const char *name) {}
	virtual void onUserStatus(const char *name, uint32 status) {}
	virtual void onSearchResult(SEARCH_RESULT result[], int n) {}
	virtual void onRecvMessage(ICQ_MSG &msg) {}
	virtual void onContactInfoReply(CONTACT_INFO &info) {}
	virtual void onUserInfoReply(USER_INFO &info) {}
};


#endif
