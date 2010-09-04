#include "dllmodule.h"
#include "debug.h"
#include <dlfcn.h>
#include <stdio.h>
#include <string>


DllModule::DllModule()
{
	handle = NULL;
}

bool DllModule::load(const char *dl)
{
	ICQ_ASSERT(handle == NULL);

	std::string name = dl;
	name += ".so";

	handle = dlopen(name.c_str(), RTLD_NOW);
	if (!handle) {
		ICQ_LOG("%s\n", dlerror());
		return false;
	}
	return true;
}

void DllModule::unload()
{
	if (handle) {
		dlclose(handle);
		handle = NULL;
	}
}

void *DllModule::getSymbol(const char *name)
{
	ICQ_ASSERT(handle != NULL);
	return dlsym(handle, name);
}
