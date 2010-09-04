#ifndef _OFFLINE_H
#define _OFFLINE_H

#include "modulelistener.h"


class Offline : public ModuleListener {
public:
	virtual bool init(int module, MAPI *mapi, Profile *prof);
	virtual bool handleEvent(EVENT *ev, void *data);
};


#endif
