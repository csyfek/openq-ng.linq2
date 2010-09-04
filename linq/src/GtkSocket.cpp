#include "GtkSocket.h"
#include <linq/socket.h>

CGtkSocket::CGtkSocket(int fd, SocketListener *l)
{
	listener = l;
	source = fd;
	input_handler_id = 0;
}

CGtkSocket::~CGtkSocket()
{
	if (input_handler_id)
		gtk_input_remove(input_handler_id);
}

void CGtkSocket::selectEvent(int event)
{
	if (input_handler_id)
		gtk_input_remove(input_handler_id);
	
	GdkInputCondition condition;
	condition = (GdkInputCondition)0;
	if (event & Socket::READ)
		condition = (GdkInputCondition)(condition | GDK_INPUT_READ);
	if (event & Socket::WRITE)
		condition = (GdkInputCondition)(condition | GDK_INPUT_WRITE);
	if (event & Socket::EXCEPTION)
		condition = (GdkInputCondition)(condition | GDK_INPUT_EXCEPTION);
	input_handler_id = gtk_input_add_full(source, condition, (GdkInputFunction)on_input, NULL, this, NULL);
}

void CGtkSocket::on_input(CGtkSocket *oCGtkSocket, gint source, GdkInputCondition condition)
{
	if (condition & GDK_INPUT_READ) {
		oCGtkSocket->listener->onSocketRead();	
	}
	else if (condition & GDK_INPUT_WRITE) {
		oCGtkSocket->listener->onSocketWrite();
	}
	else if (condition & GDK_INPUT_EXCEPTION) {
		oCGtkSocket->listener->onSocketException();
	}
}
