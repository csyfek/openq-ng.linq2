#include "fdset.h"
#include "debug.h"
#include <string.h>

#ifndef _WIN32
#include <sys/ioctl.h>
#endif


SocketRegistry *Socket::socketRegistry;


Socket::Socket(int fd)
{
	sockfd = fd;

	// An existing socket, add it to the registry.
	if (fd >= 0)
		socketRegistry->addSocket(fd, this);
}

Socket::~Socket()
{
	closeSocket();
}

bool Socket::createSocket(int type)
{
	ICQ_ASSERT(sockfd < 0);

	sockfd = socket(AF_INET, type, 0);
	if (sockfd < 0)
		return false;

	socketRegistry->addSocket(sockfd, this);
	return true;
}

void Socket::closeSocket()
{
	if (sockfd >= 0) {
		socketRegistry->removeSocket(sockfd);
#ifdef _WIN32
		closesocket(sockfd);
#else
		close(sockfd);
#endif
		sockfd = -1;
	}
}

void Socket::setBlockMode(bool block)
{
	unsigned long v = block;

#ifdef _WIN32
	ioctlsocket(sockfd, FIONBIO, &v);
#else
	ioctl(sockfd, FIONBIO, &v);
#endif
}

bool Socket::connect(uint32 ip, int port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = ip;

	return (::connect(sockfd, (sockaddr *) &addr, sizeof(addr)) == 0);
}

bool Socket::bindAddress(uint32 ip, int port)
{
	sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = ip;
	addr.sin_port = htons(port);

	return (bind(sockfd, (sockaddr *) &addr, sizeof(addr)) == 0);
}

bool Socket::getSocketAddress(sockaddr_in &addr)
{
	socklen_t addrlen = sizeof(addr);
	return (getsockname(sockfd, (sockaddr *) &addr, &addrlen) == 0);
}


SocketRegistryImpl::SocketRegistryImpl()
{
	// Initialize fd table
	FD_ENTRY *e = fdTable + FD_SIZE;

	while (--e >= fdTable) {
		e->fd = -1;
		e->event = 0;
		e->timeout = 0;
		e->listener = NULL;
	}

	maxfd = -1;
}

void SocketRegistryImpl::addSocket(int fd, SocketListener *l)
{
	ICQ_LOG("Add fd=%d\n", fd);

	ICQ_ASSERT(fd >= 0 && fd < FD_SIZE);
	ICQ_ASSERT(fdTable[fd].fd < 0);

	FD_ENTRY *e = fdTable + fd;
	e->fd = fd;
	e->event = 0;
	e->timeout = 0;
	e->listener = l;

	// Update maxfd
	if (maxfd < fd)
		maxfd = fd;
}

void SocketRegistryImpl::removeSocket(int fd)
{
	ICQ_ASSERT(fd >= 0 && fd < FD_SIZE);

	FD_ENTRY *e = fdTable + fd;
	e->fd = -1;
	e->event = 0;
	e->timeout = 0;
	e->listener = NULL;

	// If this is the maxfd, update it
	if (fd == maxfd) {
		while (--e >= fdTable && e->fd < 0)
			;
		maxfd = e - fdTable;
	}
}

void SocketRegistryImpl::addEvent(int fd, int event)
{
	ICQ_ASSERT(fd >= 0 && fd < FD_SIZE);
//	ICQ_ASSERT(fdTable[fd].fd == fd);

	fdTable[fd].event |= event;
}

void SocketRegistryImpl::removeEvent(int fd, int event)
{
	ICQ_ASSERT(fd >= 0 && fd < FD_SIZE);
	ICQ_ASSERT(fdTable[fd].fd == fd);

	fdTable[fd].event &= ~event;
}

void SocketRegistryImpl::setTimeout(int fd, int secs)
{
	ICQ_ASSERT(fd >= 0 && fd < FD_SIZE);
	ICQ_ASSERT(fdTable[fd].fd == fd);

	fdTable[fd].timeout = time(NULL) + secs;
}

/*
 * Under win32, it seems that fd is not allocated continously.
 * Sorry for windows guys:'(
 */
bool SocketRegistryImpl::poll(time_t now)
{
	FdSet fdset;
	int fds[FD_SIZE];
	int nfds = 0;
	FD_ENTRY *e;

	// Set in the fd_set from 0 to maxfd
	for (e = fdTable + maxfd; e >= fdTable; e--) {
		if (e->fd < 0)
			continue;

		// Check timeout
		if (e->timeout && e->timeout <= now) {
			// Reset timer
			e->timeout = 0;
			e->listener->onSocketTimeout();
	
		} else if (e->event) {
			fdset.setFd(e->fd, e->event);
			fds[nfds++] = e->fd;
		}
	}

	int n = fdset.poll();
	if (n < 0)
		return false;

	// Returns if no sockets event
	if (n == 0)
		return true;

	// Dispatch socket events
	for (int i = 0; i < nfds; i++) {
		int events = fdset.getEventsReady(i, fds[i]);
		e = fdTable + fds[i];

		if (events & SOCKET_READ) {
			// Do not poll on this fd any more if an error occurs
			if (!e->listener->onSocketRead()) {
				e->event = 0;
				continue;
			}
		}
		if (events & SOCKET_WRITE) {
			// We remove write event mask here. clients may add it again
			// in onSocketWrite()
			removeEvent(e->fd, SOCKET_WRITE);
			e->timeout = 0;
			e->listener->onSocketWrite();
		}
	}

	return true;
}
