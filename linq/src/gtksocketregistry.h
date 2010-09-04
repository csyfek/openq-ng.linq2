#ifndef __GTK_SOCKET_REGISTRY_H__
#define __GTK_SOCKET_REGISTRY_H__


#include <linq/socketregistry.h>
#include "GtkSocket.h"
#include <map>


class GtkSocketRegistry : public SocketRegistry {
public:
	GtkSocketRegistry();
	virtual ~GtkSocketRegistry();

	virtual void add(int fd, SocketListener *l);
	virtual void remove(int fd);
	virtual void selectEvent(int fd, int event);

private:
	std::map<int, CGtkSocket *> socketMap;
};


#endif
