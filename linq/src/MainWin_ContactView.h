#ifndef __LINQ_MAINWIN_CONTACTVIEW_H__
#define __LINQ_MAINWIN_CONTACTVIEW_H__

#include <linq/udpsession.h>

#include <gtkmm/box.h>
#include <gtkmm/treeview.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/menu.h>
#include <gtkmm/treestore.h>

#include <string>


class ContactView : public Gtk::VBox
{
public:
	ContactView();
	virtual ~ContactView();

	void onUserOnline(const char *name);
	void onUserOffline(const char *name);
	void onUserStatus(const char *name, uint32 status);
	void onContactInfoReply(CONTACT_INFO &c);
protected:
	Gtk::TreeView m_buddy_treeview;
	Glib::RefPtr<Gtk::TreeStore> m_buddy_treestore;
	Gtk::ScrolledWindow m_buddy_scrolledwindow;
	Gtk::TreeView m_group_treeview;
	Glib::RefPtr<Gtk::TreeStore> m_group_treestore;
	Gtk::ScrolledWindow m_group_scrolledwindow;
	
	Gtk::Menu *buddy_left_menu;
	Gtk::Menu *buddy_right_menu;
	Gtk::Menu *buddy_group_menu;

	virtual void on_buddy_button_clicked();
	virtual void on_group_button_clicked();
	virtual bool on_buddy_treeview_event(GdkEvent *e);
	virtual bool on_group_treeview_event(GdkEvent *e);
	virtual void on_buddy_treeview_realize();
	virtual void on_group_treeview_realize();

	virtual void on_buddy_left_menu_sendmsg_activate();
	virtual void on_buddy_left_menu_sendfile_activate();
	virtual void on_buddy_left_menu_sendmail_activate();
	virtual void on_buddy_left_menu_chatlog_activate();
	virtual void on_buddy_left_menu_viewinfo_activate();
	
	virtual void on_buddy_right_menu_addGroup_activate();
	virtual void on_buddy_right_menu_onlyShowOnlineUser_activate();
	virtual void on_buddy_right_menu_delete_activate();
	virtual void on_buddy_right_menu_rename_activate();	
	
	bool buddyTreeModelForeachIterFunc(const Gtk::TreeModel::iterator &iter);
private:
	struct ContactListNodeData {
		std::string name;
		uint32 status;
	};

	struct ContactModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		ContactModelColumns() { add(icon); add(text); add(data); }

		Gtk::TreeModelColumn< Glib::RefPtr<Gdk::Pixbuf> > icon;
		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<ContactListNodeData *> data;
	};

	const ContactModelColumns m_Columns;
	
	
	void loadContactList();	

	bool getContactIter(const char *name, Gtk::TreeModel::iterator &iter);
	bool getContactIter(const char *name, Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Children &children);
};

#endif
