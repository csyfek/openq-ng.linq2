#include "config.h"
#include "MainWin_MobileView.h"
#include "LinqMain.h"
#include "intl.h"

#include <gtkmm/button.h>

MobileView::MobileView()
{
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button("mobile"));
	pack_start(*button, false, false, 0);
	show_all();
}
