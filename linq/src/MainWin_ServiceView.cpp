#include "config.h"
#include "MainWin_ServiceView.h"
#include "LinqMain.h"
#include "intl.h"

#include <gtkmm/button.h>

ServiceView::ServiceView()
{
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button("service"));
	pack_start(*button, false, false, 0);
	show_all();
}
