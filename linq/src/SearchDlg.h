#ifndef __LINQ_SEARCH_DLG_H__
#define __LINQ_SEARCH_DLG_H__

#include "LinqWindow.h"
#include <linq/udpsession.h>

#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/button.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/treeview.h>
#include <gtkmm/textview.h>
#include <gtkmm/label.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/liststore.h>

struct LinqSearchResult {
	LinqSearchResult(SEARCH_RESULT &result);
	
	std::string name;
	uint32 status;
	std::string nick;
	uint8 auth;
	uint8 gender;
	uint8 age;
};

class SearchDlg : public Gtk::Window, public LinqWindow
{
public:
	SearchDlg();
	virtual ~SearchDlg();

	void onSearchResult(SEARCH_RESULT result[], int n);
	bool onAddContactReply(const char *name, uint8 auth);
protected:
	Gtk::Notebook m_notebook;
	Gtk::Button m_back_button;
	Gtk::Button m_next_button;
	Gtk::Button m_cancel_button;
	
	Gtk::RadioButton m_radio_online_user, m_radio_custom_search, m_radio_remote_friend,
			  m_radio_group_list, m_radio_group_num, m_radio_create_group;
	Gtk::Entry m_search_custom_name_entry, m_search_custom_nick_entry, m_search_custom_email_entry;
	Gtk::Entry m_group_num_entry;
	Gtk::Entry m_create_group_name_entry, m_create_group_password_entry;
	Gtk::Label m_create_group_result_info_label;
	Gtk::Image m_add_friend_image;
	Gtk::Label m_add_friend_uin_label, m_add_friend_nick_label, m_add_friend_info_label;
	Gtk::HBox m_add_friend_hbox;
	Gtk::TextView m_add_friend_textview;
	Gtk::TreeView m_user_list_treeview;
	Glib::RefPtr<Gtk::ListStore> m_user_list_liststore;
	Gtk::Label m_user_list_info_label;
	Gtk::Button m_user_list_previous_button, m_user_list_next_button, m_user_list_detail_button, m_user_list_all_button;
	Gtk::TreeView m_server_list_treeview;
	Gtk::Label m_server_list_label;
	Gtk::TreeView m_group_type_list_treeview;
	Gtk::TreeView m_group_list_treeview;
	Gtk::Label m_group_list_info_label;
	Gtk::Entry m_group_list_password_entry;

	virtual bool on_window_delete_event (GdkEventAny* event);
	virtual void on_back_clicked ();
	virtual void on_next_clicked ();
	virtual void on_cancel_clicked ();
	virtual void on_add_friend_send_button_clicked ();
	virtual void on_user_list_detail_button_clicked ();
	virtual bool on_user_list_button_press(GdkEventButton* event);
	virtual void on_user_list_previous_button_clicked ();
	virtual void on_user_list_next_button_clicked ();
	virtual void on_user_list_all_button_clicked ();
	virtual bool on_server_list_button_press(GdkEventButton* event);
	virtual bool on_group_type_list_button_press(GdkEventButton* event);
	virtual bool on_group_list_button_press(GdkEventButton* event);
private:
	struct UserListModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		UserListModelColumns() { add(name); add(nick); add(status); add(auth); add(gender); add(age); }

		Gtk::TreeModelColumn<Glib::ustring> name;
		Gtk::TreeModelColumn<Glib::ustring> nick;
		Gtk::TreeModelColumn<uint32> status;
		Gtk::TreeModelColumn<uint8> auth;
		Gtk::TreeModelColumn<uint8> gender;
		Gtk::TreeModelColumn<uint8> age;
	};
	const UserListModelColumns m_user_list_columns;

	enum SearchPage {
		NB_CHOOSE,
		NB_CUSTOM_SEARCH,
		NB_USER_LIST,
		NB_ADD_FRIEND,
		NB_SERVER_LIST,
		NB_GROUP_TYPE_LIST,
		NB_GROUP_LIST,
		NB_GROUP_NUM,
		NB_CREATE_GROUP,
		NB_CREATE_GROUP_RESULT,
		NB_NUM,
		NB_NULL,
	};	
	enum ChooseRadio {
		NB_CHOOSE_RADIO_RANDOM_SEARCH,
		NB_CHOOSE_RADIO_CUSTOM_SEARCH,
		NB_CHOOSE_RADIO_REMOTE_FRIEND,
		NB_CHOOSE_RADIO_GROUP_LIST,
		NB_CHOOSE_RADIO_GROUP_NUM,
		NB_CHOOSE_RADIO_CREATE_GROUP,
	};

	SearchPage nb_current_page;
	ChooseRadio nb_choose_radio;
	
	std::list< std::list< LinqSearchResult > * > searchUserResultPageList;
	void clean_search_user_result();
	void refresh_search_user_result (int refresh_page);
	int search_user_result_current_page;
	
	void create_notebook();
	
	void create_nb_choose();
	void create_nb_customSearch();
	void create_nb_groupNUm();
	void create_nb_createGroup();
	void create_nb_createGroupResult();
	void create_nb_addFriend();
	void create_nb_userList();
	void create_nb_serverList();
	void create_nb_groupTypeList();
	void create_nb_groupList();
		
	SearchPage nb_user_list_next();
	SearchPage nb_server_list_next();
	SearchPage nb_group_list_next();
	SearchPage nb_group_type_list_next();
	SearchPage nb_create_group_next();
	SearchPage nb_group_num_next();		
};

#endif
