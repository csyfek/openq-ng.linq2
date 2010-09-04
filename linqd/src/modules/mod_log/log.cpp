#include "log.h"
#include "session.h"
#include "logger.h"

#ifdef _WIN32
#include <winsock.h>
#else
#include <arpa/inet.h>
#endif


Log::Log()
{
	moduleIndex = -1;
	logger = NULL;
}

void Log::onSessionEvent(EVENT *ev)
{
	ev->mapi->registerEvent(moduleIndex, EV_S_END);

	in_addr in;
	in.s_addr = ev->s->getIP();
	logger->log(LOG_INFORMATION, "%s logged in from %s",
		ev->s->getUserName(), inet_ntoa(in));
}

bool Log::handleEvent(EVENT *ev, void *data)
{
	switch (ev->event) {
	case EV_SESSION:
		onSessionEvent(ev);
		break;

	case EV_S_END:
		logger->log(LOG_INFORMATION, "%s logged out", ev->s->getUserName());
		break;
	}
	return false;
}

bool Log::init(int module, MAPI *mapi, Profile *prof)
{
	moduleIndex = module;

	logger = mapi->getLogger();

	mapi->registerEvent(module, EV_SESSION);
	return true;
}


ICQ_EXPORT ModuleListener *getModuleListener()
{
	return new Log;
}
