#ifndef _SOCKET_H
#define _SOCKET_H

#include "socketregistry.h"
#include "types.h"
#include <time.h>


#ifdef _WIN32
#include <winsock.h>

#define EWOULDBLOCK		WSAEWOULDBLOCK
#define EINPROGRESS		EWOULDBLOCK

typedef int socklen_t;

// Sorry, windows guys
#define FD_SIZE		2048

#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#define FD_SIZE		1024

#endif


/*
 * Simple socket wrapper class
 */
class Socket : public SocketListener {
public:
	static int getLastError() {
#ifdef _WIN32
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	Socket(int fd = -1);
	~Socket();

	void addEvent(int event) {
		socketRegistry->addEvent(sockfd, event);
	}
	void removeEvent(int event) {
		socketRegistry->removeEvent(sockfd, event);
	}
	void setTimeout(int timeout) {
		socketRegistry->setTimeout(sockfd, timeout);
	}

	// Create a TCP/UDP socket
	bool createSocket(int type);

	void closeSocket();

	// Connect to the remote host, returns true if connection is established,
	// otherwise, call Socket::getLastError() to get detail information
	bool connect(uint32 ip, int port);

	bool bindAddress(uint32 ip, int port);
	bool getSocketAddress(sockaddr_in &addr);

	// Default is non-block
	void setBlockMode(bool block = true);

	// This variable must be initialized before any sockets are created!
	static SocketRegistry *socketRegistry;

protected:
	int sockfd;
};


/*
 * An entry in the fd table
 */
struct FD_ENTRY {
	int fd;
	int event;
	time_t timeout;
	SocketListener *listener;
};


class SocketRegistryImpl : public SocketRegistry {
public:
	SocketRegistryImpl();

	virtual void addSocket(int fd, SocketListener *l);
	virtual void removeSocket(int fd);
	virtual void addEvent(int fd, int event);
	virtual void removeEvent(int fd, int event);
	virtual void setTimeout(int fd, int secs);

	// Poll all of the sockets registerd in the fd table
	bool poll(time_t now);

private:
	FD_ENTRY fdTable[FD_SIZE];
	int maxfd;
};


#endif
