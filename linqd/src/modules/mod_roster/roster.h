#ifndef _ROSTER_H
#define _ROSTER_H

#include "modulelistener.h"


class Roster : public ModuleListener {
public:
	virtual bool init(int module, MAPI *mapi, Profile *prof);
	virtual bool handleEvent(EVENT *ev, void *data);
};


#endif
