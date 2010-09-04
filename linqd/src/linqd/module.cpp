#include "module.h"


typedef ModuleListener *(*GETMODULE)();


Module::Module()
{
	listener = NULL;
	startOrder = 0;
	hasSessionEvent = false;
}

Module::~Module()
{
	if (listener)
		listener->destroy();
}

bool Module::load(const char *dl)
{
	if (!dllModule.load(dl))
		return false;

	// Get the entry point
	GETMODULE getModule = (GETMODULE) dllModule.getSymbol("getModuleListener");
	if (!getModule)
		return false;

	listener = getModule();
	return true;
}

void Module::unload()
{
	if (listener) {
		listener->destroy();
		listener = NULL;
	}
	dllModule.unload();
}
