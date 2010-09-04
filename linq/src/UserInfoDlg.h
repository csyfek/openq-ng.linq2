#ifndef __LINQ_USERINFO_DLG_H__
#define __LINQ_USERINFO_DLG_H__

#include "LinqWindow.h"
#include "LinqContactInfo.h"
#include <linq/udpsession.h>

#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/treeview.h>
#include <gtkmm/button.h>
#include <gtkmm/textview.h>
#include <gtkmm/entry.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/combo.h>
#include <gtkmm/optionmenu.h>

class UserInfoDlg : public Gtk::Window, public LinqWindow
{
public:
	UserInfoDlg(LinqContactInfo &c);
	UserInfoDlg(LinqUserInfo &user);
	virtual ~UserInfoDlg();	
	
	void onContactInfoReply(LinqContactInfo &c);
	void onUserInfoReply(LinqUserInfo &user);

	const std::string &getName() { return name; }
protected:
	Gtk::TreeView m_treeview;
	Gtk::Notebook m_notebook;

	Gtk::Button m_refresh_button;
	Gtk::Button m_ok_button;
	Gtk::Button m_cancel_button;

	Gtk::Entry m_name_entry;
	Gtk::Entry m_nick_entry;
	Gtk::Entry *age_entry;
	Gtk::Entry *birth_year_entry, *birth_month_entry, *birth_day_entry;
	Gtk::OptionMenu m_gender_optionmenu;
	Gtk::Combo m_country_combo;
	Gtk::Entry m_city_entry;

	Gtk::Entry m_email_entry;
	Gtk::Entry m_address_entry;
	Gtk::Entry m_postcode_entry;
	Gtk::Entry m_tel_entry;
	Gtk::Entry m_mobile_entry;
	
	Gtk::Entry m_realname_entry;
	Gtk::Combo m_occupation_combo;
	Gtk::Entry m_homepage_entry;
	Gtk::TextView m_intro_textview;

	Gtk::CheckButton *change_passwd_check_button;
	Gtk::Entry *old_passwd_entry;
	Gtk::Entry *new_passwd_entry_1, *new_passwd_entry_2;
	Gtk::RadioButton *auth_radio_1, *auth_radio_2, *auth_radio_3;

	virtual bool on_window_delete_event(GdkEventAny *event);
	virtual void on_treeview_selection_changed();
	virtual void on_change_passwd_checkbutton_toggled();
	virtual void on_change_button_clicked();
	virtual void on_refresh_button_clicked();
	virtual void on_close_button_clicked();	
private:
	std::string name;

	void createWindow(bool is_myself);
	Gtk::Widget &createLeftTree(bool is_myself);
	Gtk::Widget &createPageBasic(bool is_myself);
	Gtk::Widget &createPageContact(bool is_myself);
	Gtk::Widget &createPageDetail(bool is_myself);
	Gtk::Widget &createPageMyinfo();

	struct ModelColumns : public Gtk::TreeModel::ColumnRecord
	{
  		ModelColumns() { add(text); add(page); }

		Gtk::TreeModelColumn<Glib::ustring> text;
		Gtk::TreeModelColumn<int> page;
	};

	const ModelColumns m_Columns;

	void setContactInfo(LinqContactInfo &c);
	void setUserInfo(LinqUserInfo &user);
	
	void getUserInfo(LinqUserInfo &info);

	static void get_list_by_str(std::list<std::string> &gl, const char *str);
};

#endif
