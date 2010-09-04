#include "config.h"
#include "PrefsDlg.h"
#include "intl.h"

#include <gtkmm/box.h>
#include <gtkmm/separator.h>
#include <gtkmm/treestore.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/frame.h>
#include <gtkmm/fontselection.h>



PrefsDlg::PrefsDlg()
: LinqWindow(WIN_PREFS),
  m_nb_window_custom_font_ckbutton(_("Use custom font.")),
  m_nb_window_custom_font_hbox(false, 12),
  m_nb_network_proxy_table(5, 2, false),
  m_nb_network_proxy_resolve_ckbutton(_("resolve the host name by socks 5 proxy"))
{
	signal_delete_event().connect(SigC::slot(*this, &PrefsDlg::on_window_delete_event));
	set_resizable(false);
	set_title(_("Preferences"));
	set_border_width(5);
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 18));	
	vbox->pack_start(*hbox);
	hbox->set_border_width(10);
	
	hbox->pack_start(createLeftTree(), false, false, 0);
	
	hbox->pack_start(m_notebook, true, true, 0);
	m_notebook.set_show_tabs(false);
	m_notebook.set_show_border(false);

	LinqDB::loadOptions(options);
	LinqDB::loadGlobalSetting(setting);
	m_notebook.append_page(createPageWindow(), "");
	m_notebook.append_page(createPageNetwork(), "");
	
	Gtk::HSeparator *sep;
	sep = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*sep);
	
	hbox = Gtk::manage(new Gtk::HBox(false, 5));
	vbox->pack_start(*hbox);
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button(_("Cancel")));
	button->signal_clicked().connect(SigC::slot(*this, &PrefsDlg::on_cancel_button_clicked));
	hbox->pack_end(*button, false, false, 0);
	button = Gtk::manage(new Gtk::Button(_("OK")));
	button->signal_clicked().connect(SigC::slot(*this, &PrefsDlg::on_ok_button_clicked));
	hbox->pack_end(*button, false, false, 0);
	show_all();
}

PrefsDlg::~PrefsDlg()
{
}

Gtk::Widget &PrefsDlg::createLeftTree()
{
	Glib::RefPtr<Gtk::TreeStore> treestore;
	treestore = Gtk::TreeStore::create(m_Columns);

	Gtk::TreeModel::iterator iter = treestore->append();
	Gtk::TreeModel::Row row = *iter;	
	row[m_Columns.text] = _("Window setting");
	row[m_Columns.page] = 0;
	iter = treestore->append();
	row = *iter;	
	row[m_Columns.text] = _("Network setting");
	row[m_Columns.page] = 1;
	
	m_treeview.set_headers_visible(false);
	m_treeview.set_rules_hint(true);
	
	m_treeview.set_model(treestore);
	
	m_treeview.append_column("", m_Columns.text);
	m_treeview.get_selection()->signal_changed().connect(SigC::slot(*this, &PrefsDlg::on_treeview_selection_changed));
	
	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	sw->set_size_request (120, 180);
	
	sw->add(m_treeview);
	return *sw;
}

void PrefsDlg::on_nb_window_custom_font_ckbutton_toggled()
{
	bool active = m_nb_window_custom_font_ckbutton.get_active();
	setting.use_custom_font = active;
	m_nb_window_custom_font_hbox.set_sensitive(active);
}

void PrefsDlg::on_nb_window_custom_font_button_clicked()
{
	Gtk::FontSelectionDialog dialog;
	dialog.set_transient_for(*this);
	if (!setting.custom_font.empty())
		dialog.set_font_name(setting.custom_font.c_str());
	int result = dialog.run();
	if (result == Gtk::RESPONSE_OK) {		
		setting.custom_font = dialog.get_font_name();
		m_nb_window_custom_font_button.set_label(setting.custom_font);
	}
}

Gtk::Widget &PrefsDlg::createPageWindow()
{
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Window setting")));
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	frame->add(*vbox);
	
	vbox->pack_start(m_nb_window_custom_font_ckbutton, false, false, 0);		
	m_nb_window_custom_font_ckbutton.set_active(setting.use_custom_font);	
	
	m_nb_window_custom_font_hbox.set_sensitive(setting.use_custom_font);	
	vbox->pack_start(m_nb_window_custom_font_hbox, false, false, 0);
	
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Custom font:")));
	m_nb_window_custom_font_hbox.pack_start(*label, false, false, 0);
	label->set_alignment(0, .5);
	if (setting.custom_font.empty())
		m_nb_window_custom_font_button.set_label(_("Choose"));
	else
		m_nb_window_custom_font_button.set_label(setting.custom_font.c_str());
	m_nb_window_custom_font_hbox.pack_start(m_nb_window_custom_font_button, false, false, 0);
	m_nb_window_custom_font_button.signal_clicked().connect(SigC::slot(*this, &PrefsDlg::on_nb_window_custom_font_button_clicked));	

	m_nb_window_custom_font_ckbutton.signal_toggled().connect(SigC::slot(*this, &PrefsDlg::on_nb_window_custom_font_ckbutton_toggled));		
	return *frame;
}

void PrefsDlg::on_nb_network_proxy_type_optionmenu_changed()
{
	m_nb_network_proxy_table.set_sensitive(m_nb_network_proxy_type_optionmenu.get_history() != 0);
}

Gtk::Widget &PrefsDlg::createPageNetwork()
{
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("NetWork setting")));
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	frame->add(*vbox);

	vbox->pack_start(m_nb_network_proxy_type_optionmenu, false, false, 0);
	Gtk::Menu *menu;
	menu = Gtk::manage(new Gtk::Menu());
	Gtk::Menu::MenuList& menulist = menu->items();
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("No proxy")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("SOCKS 5")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("HTTP")));
	m_nb_network_proxy_type_optionmenu.set_menu(*menu);

	m_nb_network_proxy_table.set_sensitive(false);
	vbox->pack_start(m_nb_network_proxy_table, false, false, 0);

	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("proxy address")));
	m_nb_network_proxy_table.attach(*label, 0, 1, 0, 1);
	m_nb_network_proxy_table.attach(m_nb_network_proxy_host_entry, 1, 2, 0, 1);
	label = Gtk::manage(new Gtk::Label(_("port")));
	m_nb_network_proxy_table.attach(*label, 0, 1, 1, 2);
	m_nb_network_proxy_table.attach(m_nb_network_proxy_port_entry, 1, 2, 1, 2);
	label = Gtk::manage(new Gtk::Label(_("username")));
	m_nb_network_proxy_table.attach(*label, 0, 1, 2, 3);
	m_nb_network_proxy_table.attach(m_nb_network_proxy_name_entry, 1, 2, 2, 3);
	label = Gtk::manage(new Gtk::Label(_("password")));
	m_nb_network_proxy_table.attach(*label, 0, 1, 3, 4);
	m_nb_network_proxy_table.attach(m_nb_network_proxy_passwd_entry, 1, 2, 3, 4);
	
	m_nb_network_proxy_table.attach(m_nb_network_proxy_resolve_ckbutton, 0, 2, 4, 5);

	m_nb_network_proxy_type_optionmenu.signal_changed().connect(SigC::slot(*this, &PrefsDlg::on_nb_network_proxy_type_optionmenu_changed));
	
	m_nb_network_proxy_type_optionmenu.set_history(options.proxy_type);
	m_nb_network_proxy_host_entry.set_text(options.proxy_host);
	if (options.proxy_port) {
		gchar *port = g_strdup_printf("%d", options.proxy_port);
		m_nb_network_proxy_port_entry.set_text(port);
		g_free(port);
	}
	m_nb_network_proxy_name_entry.set_text(options.proxy_username);
	m_nb_network_proxy_passwd_entry.set_text(options.proxy_passwd);
	m_nb_network_proxy_resolve_ckbutton.set_active(options.proxy_resolve);
	return *frame;
}

void PrefsDlg::getLinqOption(LinqOption &option)
{
	option.proxy_type = m_nb_network_proxy_type_optionmenu.get_history();
	option.proxy_host = m_nb_network_proxy_host_entry.get_text();
	option.proxy_port = atoi(m_nb_network_proxy_port_entry.get_text().c_str());
	option.proxy_username = m_nb_network_proxy_host_entry.get_text();
	option.proxy_passwd = m_nb_network_proxy_host_entry.get_text();
	option.proxy_resolve = m_nb_network_proxy_resolve_ckbutton.get_active();
}

void PrefsDlg::getLinqGlobalSetting(LinqGlobalSetting &setting)
{
}

bool PrefsDlg::on_window_delete_event(GdkEventAny * event)
{
	delete this;
	return true;
}

void PrefsDlg::on_treeview_selection_changed()
{
	Gtk::TreeModel::iterator iter = m_treeview.get_selection()->get_selected();
	if (iter) {
  		Gtk::TreeModel::Row row = *iter;
		m_notebook.set_current_page(row[m_Columns.page]);
	}
}

void PrefsDlg::on_ok_button_clicked()
{
	getLinqOption(options);
	LinqDB::saveOptions(options);
	LinqDB::saveGlobalSetting(setting);
	delete this;
}

void PrefsDlg::on_cancel_button_clicked()
{
	delete this;
}
