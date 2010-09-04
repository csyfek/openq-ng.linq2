#include "config.h"
#include "MainWin_ChainView.h"
#include "LinQ.h"
#include "intl.h"

#include <gtkmm/button.h>

ChainView::ChainView()
{
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button("chain"));
	pack_start(*button, false, false, 0);
	show_all();
}
