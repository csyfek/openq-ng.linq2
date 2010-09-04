#ifndef _PRESENCE_H
#define _PRESENCE_H

#include "modulelistener.h"
#include "stringset.h"


class Presence : public ModuleListener {
public:
	virtual bool init(int module, MAPI *mapi, Profile *prof);
	virtual bool handleEvent(EVENT *ev, void *data);
};


#endif
