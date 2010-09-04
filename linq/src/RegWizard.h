#ifndef __LINQ_REGWIZARD_H__
#define __LINQ_REGWIZARD_H__

#include "LinqWindow.h"
#include "LinqOption.h"
#include <linq/udpsession.h>

#include <gtkmm/window.h>
#include <gtkmm/button.h>
#include <gtkmm/table.h>
#include <gtkmm/notebook.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/combo.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/optionmenu.h>
#include <gtkmm/textview.h>

class RegWizard : public Gtk::Window, public LinqWindow
{
public:	
	RegWizard(const char *user, const char *passwd);
	virtual ~RegWizard();

	void onConnect(bool connected);
	void onRegisterReply(uint8 error);
	void onLoginReply(uint8 error);
	void onUserInfoReply(USER_INFO &user);
	void onContactListReply(const char *contacts[], int n);
protected:
	Gtk::Button m_back_button;
	Gtk::Button m_next_button;
	Gtk::Button m_cancel_button;

	virtual bool on_delete_event(GdkEventAny* event);
	virtual void on_back_clicked();
	virtual void on_next_clicked();
	virtual void on_cancel_clicked();
		
	Gtk::Notebook m_notebook;

	Gtk::RadioButton m_nb_choose_new_user_radio;
	Gtk::Table m_nb_choose_exist_user_table;
	Gtk::Entry m_nb_choose_user_entry;
	Gtk::Entry m_nb_choose_password_entry;

	virtual void on_nb_choose_exist_user_radio_toggled();
	virtual void on_nb_choose_user_entry_activate();
	virtual void on_nb_choose_password_entry_activate();

	Gtk::Combo m_nb_network_server_host_combo;
	Gtk::Entry m_nb_network_server_port_entry;
	Gtk::OptionMenu m_nb_network_proxy_type_optionmenu;
	Gtk::Table m_nb_network_proxy_table;
	Gtk::Entry m_nb_network_proxy_host_entry;
	Gtk::Entry m_nb_network_proxy_port_entry;
	Gtk::Entry m_nb_network_proxy_name_entry;
	Gtk::Entry m_nb_network_proxy_passwd_entry;
	Gtk::CheckButton m_nb_network_proxy_resolve_ckbutton;
	
	virtual void on_nb_network_proxy_type_optionmenu_changed();
	
	
	Gtk::Entry m_nb_basic_account_entry;
	Gtk::Entry m_nb_basic_passwd_1_entry;
	Gtk::Entry m_nb_basic_passwd_2_entry;
	Gtk::Entry m_nb_basic_nickname_entry;
	Gtk::OptionMenu m_nb_basic_gender_optionmenu;
	Gtk::Entry m_nb_basic_birth_year_entry;
	Gtk::Entry m_nb_basic_birth_month_entry;
	Gtk::Entry m_nb_basic_birth_day_entry;
	Gtk::Entry m_nb_basic_country_entry;
	Gtk::Entry m_nb_basic_city_entry;
	
	
	Gtk::Entry m_nb_contact_email_entry;
	Gtk::Entry m_nb_contact_address_entry;
	Gtk::Entry m_nb_contact_postcode_entry;
	Gtk::Entry m_nb_contact_tel_entry;
	Gtk::Entry m_nb_contact_mobile_entry;
	
	Gtk::Entry m_nb_detail_realname_entry;
	Gtk::Combo m_nb_detail_occupation_combo;
	Gtk::Entry m_nb_detail_homepage_entry;
	Gtk::TextView m_nb_detail_intro_textview;
	Gtk::RadioButton m_nb_detail_auth_radio_1, m_nb_detail_auth_radio_2;	
	
	
	Gtk::Label m_nb_register_result_label;
private:
	gboolean UserInfo_received;
	gboolean ContactList_received;

	enum RegPage {
		NB_CHOOSE,
		NB_BASIC_INFO,
		NB_CONTACT_INFO,
		NB_DETAIL_INFO,
		NB_NETWORK_INFO,
		NB_REGISTER_RESULT,
		NB_NUM,
		NB_NULL,
	};

	RegPage nb_current_page;

	bool isFinished;
	void setFinished();
	
	void getUserInfo(LinqUserInfo &info);
	void getLinqOption(LinqOption &option);

	void create_nb_choose(const char *user, const char *passwd);
	void create_nb_basic_info();
	void create_nb_contact_info();
	void create_nb_detail_info();
	void create_nb_network_info();
	void create_nb_register_result();
	
	RegPage nb_choose_next();
	RegPage nb_basic_info_next();
	RegPage nb_contact_info_next();
	RegPage nb_detail_info_next();
	RegPage nb_network_info_next();
	RegPage nb_register_result_next();	
};

#endif
