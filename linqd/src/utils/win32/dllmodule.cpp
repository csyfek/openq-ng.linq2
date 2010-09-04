#include "dllmodule.h"
#include "debug.h"


DllModule::DllModule()
{
	hModule = NULL;
}

bool DllModule::load(const char *name)
{
	ICQ_ASSERT(hModule == NULL);

	hModule = LoadLibrary(name);
	return (hModule != NULL);
}

void DllModule::unload()
{
	if (hModule) {
		FreeLibrary(hModule);
		hModule = NULL;
	}
}

void *DllModule::getSymbol(const char *name)
{
	ICQ_ASSERT(hModule != NULL);

	return GetProcAddress(hModule, name);
}
