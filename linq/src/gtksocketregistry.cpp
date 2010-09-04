#include "gtksocketregistry.h"


GtkSocketRegistry::GtkSocketRegistry()
{
}

GtkSocketRegistry::~GtkSocketRegistry()
{
	std::map<int, CGtkSocket *>::iterator iter;
	for (iter = socketMap.begin(); iter != socketMap.end(); ++iter)
		delete (*iter).second;
}

void GtkSocketRegistry::add(int fd, SocketListener *l)
{	
	std::map<int, CGtkSocket *>::iterator iter;
	iter = socketMap.find(fd);
	if (iter!= socketMap.end()) //found
		return;
	socketMap[fd] = new CGtkSocket(fd, l);
}

void GtkSocketRegistry::remove(int fd)
{
	std::map<int, CGtkSocket *>::iterator iter;
	iter = socketMap.find(fd);
	if (iter== socketMap.end()) //not found
		return;	
	delete (*iter).second;
	socketMap.erase(iter);
}

void GtkSocketRegistry::selectEvent(int fd, int event)
{
	std::map<int, CGtkSocket *>::iterator iter;
	iter = socketMap.find(fd);
	if (iter== socketMap.end()) //not found
		return;	
	(*iter).second->selectEvent(event);
}
