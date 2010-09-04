#ifndef _FD_SET_H
#define _FD_SET_H

#include "config.h"
#include "socket.h"

#ifdef HAVE_POLL
#include <sys/poll.h>
#endif


class FdSet {
public:
	FdSet();

	void setFd(int fd, int events);
	int getEventsReady(int i, int fd);
	int poll();

private:
#ifdef HAVE_POLL
	pollfd ufds[FD_SIZE];
	int size;
#else
	fd_set readfds, writefds;
	int maxfd;
#endif
};

// Default poll timeout in milliseconds, should it be passed as a parameter?
#define POLL_TIMEOUT	500

#ifdef HAVE_POLL
#include "poll.cpp"
#else
#include "select.cpp"
#endif

#endif

