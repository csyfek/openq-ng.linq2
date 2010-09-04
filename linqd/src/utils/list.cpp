#include "list.h"


void ListHead::remove()
{
	prev->next = next;
	next->prev = prev;
	prev = next = this;
}

ListHead *ListHead::removeHead()
{
	ListHead *t = next;
	next->remove();
	return t;
}

void ListHead::add(ListHead *item)
{
	item->prev = prev;
	item->next = this;
	prev->next = item;
	prev = item;
}

void ListHead::addHead(ListHead *item)
{
	item->prev = this;
	item->next = next;
	next->prev = item;
	next = item;
}
