#ifndef __LINQ_SYSMSGLIST_DLG_H__
#define __LINQ_SYSMSGLIST_DLG_H__

#include "LinqWindow.h"
#include <gtkmm/window.h>
#include <gtkmm/treeview.h>


class SysMsgListDlg : public Gtk::Window, public LinqWindow
{
public:
	SysMsgListDlg();
	virtual ~SysMsgListDlg();
protected:
	Gtk::TreeView m_treeview;

	virtual bool on_window_delete_event(GdkEventAny *event);
	virtual bool on_treeview_button_press(GdkEventButton* event);
	virtual void on_delete_button_clicked();
	virtual void on_deleteAll_button_clicked();
	virtual void on_close_button_clicked();
private:
	struct ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		ModelColumns() { add(from); add(date); add(time); add(content); }

		Gtk::TreeModelColumn<Glib::ustring> from;
		Gtk::TreeModelColumn<Glib::ustring> date;
		Gtk::TreeModelColumn<Glib::ustring> time;
		Gtk::TreeModelColumn<Glib::ustring> content;
	};

	const ModelColumns m_Columns;

	void createTreeView();
};

#endif
