#ifndef _SEARCH_H
#define _SEARCH_H

#include "modulelistener.h"
#include "sessionmanager.h"


class Search : public ModuleListener {
public:
	Search();
	~Search();

	virtual bool init(int module, MAPI *mapi, Profile *prof);
	virtual bool handleEvent(EVENT *ev, void *data);

private:
	bool onSessionEvent(EVENT *ev);
	bool onSessionOut(EVENT *ev);
	bool onSessionIn(EVENT *ev);

	SessionManager *sessionManager;
};


#endif
