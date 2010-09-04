#ifndef _VCARD_H
#define _VCARD_H

#include "modulelistener.h"


class VCard : public ModuleListener {
public:
	virtual bool init(int module, MAPI *mapi, Profile *prof);
	virtual bool handleEvent(EVENT *ev, void *data);
};


#endif
