#ifndef _DLL_MODULE_H
#define _DLL_MODULE_H

#ifdef _WIN32
#include <windows.h>
#endif

/*
 * A simple wrapper for dynamic link library loading
 */
class DllModule {
public:
	DllModule();
	~DllModule() { unload(); }

	bool load(const char *name);
	void unload();
	void *getSymbol(const char *name);

private:
#ifdef _WIN32
	HMODULE hModule;
#else
	void *handle;
#endif
};


#endif
