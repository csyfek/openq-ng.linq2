#ifndef _MESSAGE_H
#define _MESSAGE_H

#include "modulelistener.h"


class Message : public ModuleListener {
public:
	virtual bool init(int module, MAPI *mapi, Profile *prof);
	virtual bool handleEvent(EVENT *ev, void *data);
};


#endif
