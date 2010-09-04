#ifndef _SOCKET_REGISTRY_H
#define _SOCKET_REGISTRY_H


enum {
	SOCKET_READ = 0x01,
	SOCKET_WRITE = 0x02,
};


class SocketListener {
public:
	virtual bool onSocketRead() { return false; }
	virtual void onSocketWrite() {}
	virtual void onSocketTimeout() {}
};


class SocketRegistry {
public:
	virtual void addSocket(int fd, SocketListener *l) = 0;
	virtual void removeSocket(int fd) = 0;
	virtual void addEvent(int fd, int event) = 0;
	virtual void removeEvent(int fd, int event) = 0;

	// If secs is 0, disable timer
	virtual void setTimeout(int fd, int secs) = 0;
};


#endif
