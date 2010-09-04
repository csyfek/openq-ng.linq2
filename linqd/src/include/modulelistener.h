#ifndef _MODULE_LISTENER_H
#define _MODULE_LISTENER_H

#include "mapi.h"
#include "profile.h"
#include "event.h"


class ModuleListener {
public:
	virtual ~ModuleListener() {}
	virtual void destroy() { delete this; }

	// Called when the module is loaded
	virtual bool init(int module, MAPI *mapi, Profile *prof) = 0;

	// Called when a registered event occurs
	// Returns true if this event is processed, and you do not want
	// subsequent modules to receive it
	virtual bool handleEvent(EVENT *ev, void *data) { return false; }
};


#endif
