#include "debug.h"


inline FdSet::FdSet()
{
	size = 0;
}

inline void FdSet::setFd(int fd, int events)
{
	ICQ_ASSERT(size < FD_SIZE);

	pollfd *p = &ufds[size++];
	p->fd = fd;
	p->events = p->revents = 0;

	if (events & SOCKET_READ)
		p->events |= POLLIN;
	if (events & SOCKET_WRITE)
		p->events |= POLLOUT;

	ICQ_ASSERT(p->events != 0);
}

inline int FdSet::getEventsReady(int i, int fd)
{
	ICQ_ASSERT(i < size);

	int events = 0;
	int revents = ufds[i].revents;

	if (revents & (POLLIN | POLLERR))
		events |= SOCKET_READ;
	if (revents & POLLOUT)
		events |= SOCKET_WRITE;

	return events;
}

inline int FdSet::poll()
{
	int n = ::poll(ufds, size, POLL_TIMEOUT);
	if (n < 0 && errno == EINTR)
		return 0;

	return n;
}
