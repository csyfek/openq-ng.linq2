#ifndef __LINQ_MSG_MANAGER_H__
#define __LINQ_MSG_MANAGER_H__

#include "LinqWindow.h"

#include <gtkmm/window.h>
#include <gtkmm/treeview.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/liststore.h>


class MsgManager : public Gtk::Window, public LinqWindow
{
public:
	MsgManager(const char *name = NULL);
	~MsgManager();
	
	void doSelect(const char *name);
protected:	
	virtual bool on_window_delete_event(GdkEventAny *event);

	virtual void on_menu_file_saveAs();
	virtual void on_menu_file_close();
	virtual void on_menu_edit_delete();
	virtual void on_menu_view_firstPage();
	virtual void on_menu_view_previousPage();
	virtual void on_menu_view_nextPage();
	virtual void on_menu_view_lastPage();

	Gtk::TreeView m_RosterTreeview;
	virtual void on_RosterTreeview_selection_changed();

	Gtk::Widget *page_msg;
	Gtk::Label *page_msg_label;
	Gtk::Widget *page_info;
	Gtk::Widget *page_remark;
	Gtk::TreeView m_MsgTreeview;
	Glib::RefPtr<Gtk::ListStore> m_MsgListstore;
	virtual void on_MsgTreeview_selection_changed();
	Gtk::TreeView m_InfoTreeview;
	Glib::RefPtr<Gtk::ListStore> m_InfoListstore;
	Gtk::TreeView m_RemarkTreeview;
	Glib::RefPtr<Gtk::ListStore> m_RemarkListstore;
	Gtk::Image m_MsgViewer_sender_icon;
	Gtk::Label m_MsgViewer_sender_label;
	Gtk::TextView m_MsgViewer_textview;
private:
	enum SeletedItemType {
		SELECT_ITEM_TYPE_BUDDY,
		SELECT_ITEM_TYPE_SYSMSG,
	};
	SeletedItemType nowSeletedItemType;
	struct RosterModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		RosterModelColumns() { add(icon); add(text); add(name); }

		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > icon;
		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<Glib::ustring> name;
	};
	const RosterModelColumns m_RosterColumns;

	struct MsgModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		MsgModelColumns() { add(sender); add(date); add(time); add(content); }

		Gtk::TreeModelColumn<Glib::ustring> sender;
		Gtk::TreeModelColumn<Glib::ustring> date;
		Gtk::TreeModelColumn<Glib::ustring> time;
		Gtk::TreeModelColumn<Glib::ustring> content;
	};
	const MsgModelColumns m_MsgColumns;

	struct InfoModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		InfoModelColumns() { add(item); add(content); }

		Gtk::TreeModelColumn<Glib::ustring> item;
		Gtk::TreeModelColumn<Glib::ustring> content;
	};
	const InfoModelColumns m_InfoColumns;
	
	Gtk::Widget &createMenu();
	Gtk::Widget &createToolbar();
	Gtk::Widget &createLeftTree();
	Gtk::Widget *createMsgList();
	Gtk::Widget *createInfoList();
	Gtk::Widget *createRemarkList();
	Gtk::Widget &createMsgViewer();
};

#endif
