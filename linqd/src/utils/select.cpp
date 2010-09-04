#include "debug.h"


inline FdSet::FdSet()
{
	maxfd = -1;

	FD_ZERO(&readfds);
	FD_ZERO(&writefds);
}

inline void FdSet::setFd(int fd, int events)
{
	if (events & SOCKET_READ)
		FD_SET(fd, &readfds);
	if (events & SOCKET_WRITE)
		FD_SET(fd, &writefds);

	if (maxfd < fd)
		maxfd = fd;
}

inline int FdSet::getEventsReady(int i, int fd)
{
	int events = 0;

	if (FD_ISSET(fd, &readfds))
		events |= SOCKET_READ;
	if (FD_ISSET(fd, &writefds))
		events |= SOCKET_WRITE;

	return events;
}

inline int FdSet::poll()
{
#ifdef _WIN32
	// Under win32, if there are no sockets in the fd_set, select() will fail.
	// In this case, we sleep a period of time and return.
	if (maxfd < 0) {
		Sleep(POLL_TIMEOUT);
		return 0;
	}
#endif
	// Why set them every time instead of once at start-up time?
	// Under linux, timeval may be modified
	timeval tv;
	tv.tv_sec = POLL_TIMEOUT / 1000;
	tv.tv_usec = (POLL_TIMEOUT % 1000) * 1000;

	int n = select(maxfd + 1, &readfds, &writefds, NULL, &tv);
	if (n < 0) {
#ifndef _WIN32
		// Just an interrupt signal, ignore it
		if (errno == EINTR)
			return 0;
#endif
		ICQ_LOG("select() failed: %d\n", Socket::getLastError());
	}
	return n;
}
