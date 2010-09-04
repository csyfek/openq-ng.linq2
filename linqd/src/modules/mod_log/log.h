#ifndef _LOG_H
#define _LOG_H

#include "modulelistener.h"


class Log : public ModuleListener {
public:
	Log();

	virtual bool init(int module, MAPI *mapi, Profile *prof);
	virtual bool handleEvent(EVENT *ev, void *data);

private:
	void onSessionEvent(EVENT *ev);

	int moduleIndex;
	Logger *logger;
};


#endif
