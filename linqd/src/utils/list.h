#ifndef _LIST_H
#define _LIST_H

/*
 * Generic doubly-linked list
 */

// The following macros are stolen from Linux :-)

#define LIST_ENTRY(ptr, type, member) \
	((type *) ((char *) (ptr) - (unsigned long) (&((type *) 0)->member)))

#define LIST_FOR_EACH(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)


class ListHead {
public:
	ListHead() { prev = next = this; }

	bool isEmpty() { return (next == this); }

	// Remove and returns the first node
	ListHead *removeHead();

	// Append to the list
	void add(ListHead *item);
	// Add to the beginning of the list
	void addHead(ListHead *item);
	// Remove this node from the list
	void remove();

	ListHead *prev, *next;
};


#endif
