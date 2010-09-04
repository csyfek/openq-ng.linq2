#ifndef _ADMIN_H
#define _ADMIN_H

#include "modulelistener.h"
#include "socket.h"
#include "list.h"


class Admin : public Socket, public ModuleListener {
public:
	Admin();
	virtual ~Admin();

	virtual bool init(int module, MAPI *mapi, Profile *prof);

	virtual bool onSocketRead();

private:
	ListHead clientList;
	MAPI *mapi;
};


#endif
