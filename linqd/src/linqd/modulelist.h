#ifndef _MODULE_LIST_H
#define _MODULE_LIST_H

#include "module.h"

class Pool;

// List entry
struct M_LIST {
	M_LIST *next;
	Module *m;
	void *data;
};


/*
 * An ordered module list for dispatch event
 */
class ModuleList {
public:
	ModuleList() { list = NULL; }

	void add(Pool *p, Module *m, void *data);
	void remove(Module *m);
	bool dispatch(EVENT *ev);
	// Combine to list by order, then dispath
	bool dispatch(ModuleList *l, EVENT *ev);

private:
	M_LIST *list;
};


#endif
