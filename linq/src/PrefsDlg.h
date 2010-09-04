#ifndef __LINQ_PREFS_DLG_H__
#define __LINQ_PREFS_DLG_H__

#include "LinqWindow.h"
#include "LinqOption.h"
#include "LinqGlobalSetting.h"
#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/table.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>


class PrefsDlg : public Gtk::Window, public LinqWindow
{
public:
	PrefsDlg::PrefsDlg();
	PrefsDlg::~PrefsDlg();
protected:
	Gtk::TreeView m_treeview;
	Gtk::Notebook m_notebook;

	virtual bool on_window_delete_event(GdkEventAny *event);
	virtual void on_treeview_selection_changed();
	virtual void on_ok_button_clicked();
	virtual void on_cancel_button_clicked();

	Gtk::CheckButton m_nb_window_custom_font_ckbutton;
	Gtk::Button m_nb_window_custom_font_button;
	Gtk::HBox m_nb_window_custom_font_hbox;
	virtual void on_nb_window_custom_font_ckbutton_toggled();
	virtual void on_nb_window_custom_font_button_clicked();

	Gtk::OptionMenu m_nb_network_proxy_type_optionmenu;
	Gtk::Table m_nb_network_proxy_table;
	Gtk::Entry m_nb_network_proxy_host_entry;
	Gtk::Entry m_nb_network_proxy_port_entry;
	Gtk::Entry m_nb_network_proxy_name_entry;
	Gtk::Entry m_nb_network_proxy_passwd_entry;
	Gtk::CheckButton m_nb_network_proxy_resolve_ckbutton;
	
	virtual void on_nb_network_proxy_type_optionmenu_changed();
private:
	struct ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		ModelColumns() { add(text); add(page); }

		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<int> page;
	};

	const ModelColumns m_Columns;

	Gtk::Widget &createLeftTree();
	Gtk::Widget &createPageWindow();
	Gtk::Widget &createPageNetwork();
	
	LinqOption options;
	void getLinqOption(LinqOption &option);
	
	LinqGlobalSetting setting;
	void getLinqGlobalSetting(LinqGlobalSetting &setting);
};

#endif
