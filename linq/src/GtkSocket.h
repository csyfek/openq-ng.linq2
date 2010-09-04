#ifndef __LINQ_GTKSOCKET_H__
#define __LINQ_GTKSOCKET_H__

#include <gtk/gtk.h>

class SocketListener;

class CGtkSocket {
public:
	CGtkSocket(int fd, SocketListener *l);
	virtual ~CGtkSocket();

	void selectEvent(int event);
private:
	gint source;
	SocketListener *listener;
	guint input_handler_id;
	static void on_input(CGtkSocket *oCGtkSocket, gint source, GdkInputCondition condition);
};

#endif
