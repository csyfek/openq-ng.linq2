#ifndef _MODULE_H
#define _MOUDLE_H

#include "modulelistener.h"
#include "dllmodule.h"
#include <string>


class Module {
public:
	Module();
	~Module();

	bool load(const char *dl);
	void unload();

	std::string name;
	int startOrder;
	ModuleListener *listener;
	bool hasSessionEvent;

private:
	DllModule dllModule;
};


#endif
