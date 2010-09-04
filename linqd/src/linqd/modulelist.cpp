#include "modulelist.h"
#include "pool.h"


void ModuleList::add(Pool *pool, Module *m, void *data)
{
	M_LIST *n = (M_LIST *) pool->alloc(sizeof(M_LIST));
	n->m = m;
	n->data = data;

	if (!list) {	// list is empty
		list = n;
		n->next = NULL;
	} else {
		M_LIST *p = list;
		while (p->next && m->startOrder > p->next->m->startOrder)
			p = p->next;

		n->next = p->next;
		p->next = n;
	}
}

void ModuleList::remove(Module *m)
{
	if (!list)
		return;

	if (list->m == m)
		list = list->next;
	else {
		M_LIST *p = list;
		while (p->next && p->next->m != m)
			p = p->next;

		if (p->next)
			p->next = p->next->next;
	}
}

bool ModuleList::dispatch(EVENT *ev)
{
	// Dispatch event by order
	for (M_LIST *n = list; n; n = n->next) {
		if (n->m->listener->handleEvent(ev, n->data))
			return true;
	}
	return false;
}

bool ModuleList::dispatch(ModuleList *l, EVENT *ev)
{
	M_LIST *m = l->list;
	M_LIST *n = list;
	M_LIST *p;

	while (m && n) {
		if (m->m->startOrder < n->m->startOrder) {
			p = m;
			m = m->next;
		} else {
			p = n;
			n = n->next;
		}
		if (p->m->listener->handleEvent(ev, p->data))
			return true;
	}

	for (; m; m = m->next) {
		if (m->m->listener->handleEvent(ev, m->data))
			return true;
	}
	for (; n; n = n->next) {
		if (n->m->listener->handleEvent(ev, n->data))
			return true;
	}
	return false;
}
