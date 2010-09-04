#include "config.h"
#include "RegWizard.h"
#include "LinqMain.h"
#include "LinqContactInfo.h"
#include "LinqUserList.h"
#include "sha.h"
#include "intl.h"

#include <gtkmm/menu.h>
#include <gtkmm/separator.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>

RegWizard::RegWizard(const char *user, const char *passwd)
: LinqWindow(WIN_REG_WIZARD),
  m_back_button(_("< Back")),
  m_next_button(_("Next >")),
  m_cancel_button(_("Cancel")),
  m_nb_choose_new_user_radio(_("Regster new LinQ user")),
  m_nb_choose_exist_user_table(2, 2, false),
  m_nb_network_proxy_table(5, 2, false),
  m_nb_network_proxy_resolve_ckbutton(_("resolve the host name by socks 5 proxy")),
  m_nb_detail_auth_radio_1(_("anyone can add me to their contact lists")),
  m_nb_detail_auth_radio_2(_("authorization is required"))
{
	UserInfo_received = false;
	ContactList_received = false;
	isFinished = false;

	set_border_width(2);
	set_default_size (200, 160);
	set_title(_("LinQ Register wizard"));
	set_icon(linqMain->skin->linq.icon);
	set_position (Gtk::WIN_POS_CENTER);
	signal_delete_event().connect(SigC::slot(*this, &RegWizard::on_delete_event));
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);
	
	m_notebook.set_show_tabs(false);
	m_notebook.set_show_border(false);

	create_nb_choose(user, passwd);
	create_nb_basic_info();
	create_nb_contact_info();
	create_nb_detail_info();
	create_nb_network_info();
	create_nb_register_result();
	
	vbox->pack_start(m_notebook, false, false, 10);
	Gtk::HSeparator *separator;
	separator = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*separator, false, true, 10);
	
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 10));
	vbox->pack_start(*hbox, false, true, 10);
	
	m_back_button.set_sensitive(false);
	hbox->pack_start(m_back_button, false, false, 0);
	m_back_button.signal_clicked().connect(SigC::slot(*this, &RegWizard::on_back_clicked));

	hbox->pack_start(m_next_button, false, false, 0);
	m_next_button.signal_clicked().connect(SigC::slot(*this, &RegWizard::on_next_clicked));
	
	hbox->pack_start(m_cancel_button, false, false, 0);
	m_cancel_button.signal_clicked().connect(SigC::slot(*this, &RegWizard::on_cancel_clicked));

	m_notebook.set_current_page(NB_CHOOSE);
	nb_current_page = NB_CHOOSE;
	
	show_all();
}

RegWizard::~RegWizard()
{
}

void RegWizard::on_nb_choose_exist_user_radio_toggled()
{
	bool sensitive = !m_nb_choose_new_user_radio.get_active();
	m_nb_choose_exist_user_table.set_sensitive(sensitive);	
	if (sensitive)
		m_nb_choose_user_entry.grab_focus();
}

void RegWizard::on_nb_choose_user_entry_activate()
{
	m_nb_choose_password_entry.grab_focus();
}

void RegWizard::on_nb_choose_password_entry_activate()
{
	on_next_clicked();
}

void RegWizard::create_nb_choose(const char *user, const char *passwd)
{
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Register user")));
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	frame->add(*vbox);
	vbox->pack_start(m_nb_choose_new_user_radio, false, false, 0);
	Gtk::HSeparator *separator = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*separator, false, true, 10);
	Gtk::RadioButton *radio;
	radio = Gtk::manage(new Gtk::RadioButton(_("Use Exist LinQ user")));
	Gtk::RadioButton::Group group = m_nb_choose_new_user_radio.get_group();
	radio->set_group(group);
	radio->signal_toggled().connect(SigC::slot(*this, &RegWizard::on_nb_choose_exist_user_radio_toggled));
	vbox->pack_start(*radio, false, false, 0);
	m_nb_choose_exist_user_table.set_sensitive(false);
	vbox->pack_start(m_nb_choose_exist_user_table, false, false, 0);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("User name:")));
	m_nb_choose_exist_user_table.attach(*label, 0, 1, 0, 1);
	m_nb_choose_user_entry.signal_activate().connect(SigC::slot(*this, &RegWizard::on_nb_choose_user_entry_activate));
	m_nb_choose_exist_user_table.attach(m_nb_choose_user_entry, 1, 2, 0, 1);
	label = Gtk::manage(new Gtk::Label(_("Password:")));
	m_nb_choose_exist_user_table.attach(*label, 0, 1, 1, 2);
	m_nb_choose_password_entry.signal_activate().connect(SigC::slot(*this, &RegWizard::on_nb_choose_password_entry_activate));
	m_nb_choose_password_entry.set_visibility(false);
	m_nb_choose_exist_user_table.attach(m_nb_choose_password_entry, 1, 2, 1, 2);
	
	if (user != NULL) {
		radio->set_active(true);
		m_nb_choose_user_entry.set_text(user);		
		if (passwd != NULL)
			m_nb_choose_password_entry.set_text(passwd);
	}
	else {
		m_nb_choose_new_user_radio.set_active(true);
	}
	
	m_notebook.append_page(*frame, "");
}

void RegWizard::create_nb_basic_info()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Account")));
	vbox->pack_start(*frame, false, false, 0);
	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(3, 2, false));
	frame->add(*table);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Account:")));
	table->attach(*label, 0, 1, 0, 1);
	table->attach(m_nb_basic_account_entry, 1, 2, 0, 1);
	label = Gtk::manage(new Gtk::Label(_("Password:")));
	table->attach(*label, 0, 1, 1, 2);
	m_nb_basic_passwd_1_entry.set_visibility(false);
	table->attach(m_nb_basic_passwd_1_entry, 1, 2, 1, 2);
	label = Gtk::manage(new Gtk::Label(_("Password(again):")));
	table->attach(*label, 0, 1, 2, 3);
	m_nb_basic_passwd_2_entry.set_visibility(false);
	table->attach(m_nb_basic_passwd_2_entry, 1, 2, 2, 3);

	frame = Gtk::manage(new Gtk::Frame(_("Basic information")));
	vbox->pack_start(*frame, false, false, 0);
	table = Gtk::manage(new Gtk::Table(5, 2, false));
	frame->add(*table);
	label = Gtk::manage(new Gtk::Label(_("Nick name")));
	table->attach(*label, 0, 1, 0, 1);
	table->attach(m_nb_basic_nickname_entry, 1, 2, 0, 1);
	label = Gtk::manage(new Gtk::Label(_("Gender")));
	table->attach(*label, 0, 1, 1, 2);
	Gtk::Menu *menu;
	menu = Gtk::manage(new Gtk::Menu());
	Gtk::Menu::MenuList& menulist = menu->items();
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Unspecified")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Male")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Female")));
	m_nb_basic_gender_optionmenu.set_menu(*menu);
	table->attach(m_nb_basic_gender_optionmenu, 1, 2, 1, 2);
	label = Gtk::manage(new Gtk::Label(_("Birth")));
	table->attach(*label, 0, 1, 2, 3);
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	table->attach(*hbox, 1, 2, 2, 3);
	m_nb_basic_birth_year_entry.set_width_chars(4);
	hbox->pack_start(m_nb_basic_birth_year_entry, false, false, 0);
	m_nb_basic_birth_month_entry.set_width_chars(2);
	hbox->pack_start(m_nb_basic_birth_month_entry, false, false, 0);
	m_nb_basic_birth_day_entry.set_width_chars(2);
	hbox->pack_start(m_nb_basic_birth_day_entry, false, false, 0);
	label = Gtk::manage(new Gtk::Label(_("Country")));
	table->attach(*label, 0, 1, 3, 4);
	table->attach(m_nb_basic_country_entry, 1, 2, 3, 4);
	label = Gtk::manage(new Gtk::Label(_("City")));
	table->attach(*label, 0, 1, 4, 5);
	table->attach(m_nb_basic_city_entry, 1, 2, 4, 5);

	m_notebook.append_page(*vbox, "");
}

void RegWizard::create_nb_contact_info()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Connection method")));
	vbox->pack_start(*frame, false, false, 0);

	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(5, 2, false));
	frame->add(*table);

	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Email")));	
	table->attach(*label, 0, 1, 0, 1);
	table->attach(m_nb_contact_email_entry, 1, 2, 0, 1);
	label = Gtk::manage(new Gtk::Label(_("Address")));	
	table->attach(*label, 0, 1, 1, 2);
	table->attach(m_nb_contact_address_entry, 1, 2, 1, 2);
	label = Gtk::manage(new Gtk::Label(_("Post code")));	
	table->attach(*label, 0, 1, 2, 3);
	table->attach(m_nb_contact_postcode_entry, 1, 2, 2, 3);
	label = Gtk::manage(new Gtk::Label(_("Telephone")));	
	table->attach(*label, 0, 1, 3, 4);
	table->attach(m_nb_contact_tel_entry, 1, 2, 3, 4);
	label = Gtk::manage(new Gtk::Label(_("Mobile")));	
	table->attach(*label, 0, 1, 4, 5);
	table->attach(m_nb_contact_mobile_entry, 1, 2, 4, 5);

	m_notebook.append_page(*vbox, "");
}

void RegWizard::create_nb_detail_info()
{
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Detail information")));

	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	frame->add(*vbox);

	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(2, 2, false));
	vbox->pack_start(*table, false, false, 0);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Real name")));
	table->attach(*label, 0, 1, 0, 1);
	table->attach(m_nb_detail_realname_entry, 0, 1, 1, 2);
	label = Gtk::manage(new Gtk::Label(_("Occupation")));
	table->attach(*label, 1, 2, 0, 1);
	table->attach(m_nb_detail_occupation_combo, 1, 2, 1, 2);

	label = Gtk::manage(new Gtk::Label(_("Home page:")));
	vbox->pack_start(*label, false, false, 0);
	vbox->pack_start(m_nb_detail_homepage_entry, false, false, 0);

	label = Gtk::manage(new Gtk::Label(_("Introduce:")));
	vbox->pack_start(*label, false, false, 0);
	
	m_nb_detail_intro_textview.set_size_request(-1, 50);

	Gtk::ScrolledWindow *scrolled_window;
	scrolled_window = Gtk::manage(new Gtk::ScrolledWindow());
	scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrolled_window->add(m_nb_detail_intro_textview);
	scrolled_window->set_shadow_type(Gtk::SHADOW_IN);
	m_nb_detail_intro_textview.set_left_margin(2);
	m_nb_detail_intro_textview.set_right_margin(2);

	vbox->pack_start(*scrolled_window, false, false, 0);

	Gtk::Frame *frame1;
	frame1 = Gtk::manage(new Gtk::Frame(_("Authorization")));
	frame1->set_border_width(5);
	vbox->pack_start(*frame1, false, false, 0);

	Gtk::VBox *vbox2;
	vbox2 = Gtk::manage(new Gtk::VBox(false, 1));
	frame1->add(*vbox2);
	
	vbox2->pack_start(m_nb_detail_auth_radio_1, false, false, 0);
	vbox2->pack_start(m_nb_detail_auth_radio_2, false, false, 0);
	Gtk::RadioButton *radio;
	radio = Gtk::manage(new Gtk::RadioButton(_("none can add me to their contact lists")));
	vbox2->pack_start(*radio, false, false, 0);
	Gtk::RadioButton::Group group = m_nb_detail_auth_radio_1.get_group();
	m_nb_detail_auth_radio_2.set_group(group);
	radio->set_group(group);
	m_nb_detail_auth_radio_1.set_active(true);

	m_notebook.append_page(*frame, "");
}

void RegWizard::on_nb_network_proxy_type_optionmenu_changed()
{
	m_nb_network_proxy_table.set_sensitive(m_nb_network_proxy_type_optionmenu.get_history() != 0);
}

void RegWizard::create_nb_network_info()
{
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("NetWork setting")));
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	frame->add(*vbox);

	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(2, 2, false));	
	vbox->pack_start(*table, false, false, 0);

	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("server address")));	
	table->attach(*label, 0, 1, 0, 1);
	std::list<std::string> gl;
	gl.push_back(_("localhost"));
	gl.push_back(_("myicq.cosoft.org.cn"));
	gl.push_back(_("www.linuxfans.org.cn"));	
	m_nb_network_server_host_combo.set_popdown_strings(gl);
	table->attach(m_nb_network_server_host_combo, 0, 1, 1, 2);

	label = Gtk::manage(new Gtk::Label(_("port")));
	table->attach(*label, 1, 2, 0, 1);
	m_nb_network_server_port_entry.set_text("8000");
	table->attach(m_nb_network_server_port_entry, 1, 2, 1, 2);

	vbox->pack_start(m_nb_network_proxy_type_optionmenu, false, false, 0);
	Gtk::Menu *menu;
	menu = Gtk::manage(new Gtk::Menu());
	Gtk::Menu::MenuList& menulist = menu->items();
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("No proxy")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("SOCKS 5")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("HTTP")));
	m_nb_network_proxy_type_optionmenu.set_menu(*menu);
	m_nb_network_proxy_type_optionmenu.signal_changed().connect(SigC::slot(*this, &RegWizard::on_nb_network_proxy_type_optionmenu_changed));

	m_nb_network_proxy_table.set_sensitive(false);
	vbox->pack_start(m_nb_network_proxy_table, false, false, 0);

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

	m_nb_network_proxy_resolve_ckbutton.set_active(true);
	m_nb_network_proxy_table.attach(m_nb_network_proxy_resolve_ckbutton, 0, 2, 4, 5);

	m_notebook.append_page(*frame, "");
}

void RegWizard::create_nb_register_result()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("User information")));
	vbox->pack_start(*frame, false, false, 0);

	frame = Gtk::manage(new Gtk::Frame(_("Please wait")));
	vbox->pack_start(*frame, false, false, 0);

	frame->add(m_nb_register_result_label);

	m_notebook.append_page(*vbox, "");	
}

bool RegWizard::on_delete_event(GdkEventAny *event)
{
	delete this;
	linqMain->Quit();
	return true;
}

RegWizard::RegPage RegWizard::nb_choose_next()
{
	if (m_nb_choose_new_user_radio.get_active()) {
		return NB_BASIC_INFO;
	}
	else {
		return NB_NETWORK_INFO;
	}
}

RegWizard::RegPage RegWizard::nb_basic_info_next()
{
	Glib::ustring account;
	account = m_nb_basic_account_entry.get_text();
	if (account.empty()) {
		Gtk::MessageDialog dialog(*this, _("please input the account!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
		dialog.run();		
		return NB_NULL;
	}
	Glib::ustring password_1;
	password_1 = m_nb_basic_passwd_1_entry.get_text();
	if (password_1.empty()) {
		Gtk::MessageDialog dialog(*this, _("please input the password!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
		dialog.run();
		return NB_NULL;
	}
	Glib::ustring password_2;
	password_2 = m_nb_basic_passwd_2_entry.get_text();
	if (password_1 != password_2) {
		Gtk::MessageDialog dialog(*this, _("password don't equal!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
		dialog.run();
		return NB_NULL;
	}
	Glib::ustring nickname;
	nickname = m_nb_basic_nickname_entry.get_text();	
	if (nickname.empty()) {
		Gtk::MessageDialog dialog(*this, _("please input the nickname!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
		dialog.run();
		return NB_NULL;
	}	
	
	return NB_CONTACT_INFO;
}

RegWizard::RegPage RegWizard::nb_contact_info_next()
{
	return NB_DETAIL_INFO;
}

RegWizard::RegPage RegWizard::nb_detail_info_next()
{
	return NB_NETWORK_INFO;
}

RegWizard::RegPage RegWizard::nb_network_info_next()
{
	m_nb_register_result_label.set_text(_("connecting..."));	
	int proxy_type = m_nb_network_proxy_type_optionmenu.get_history();
	if (proxy_type == 0) {
		linqMain->setProxy(NULL);
	}
	else {
		PROXY_INFO proxy;
		if (proxy_type == 1)
			proxy.type = PROXY_SOCKS5;
		else
			proxy.type = PROXY_HTTPS;
		proxy.host = m_nb_network_proxy_host_entry.get_text();
		proxy.port = atoi(m_nb_network_proxy_port_entry.get_text().c_str());
		proxy.user = m_nb_network_proxy_name_entry.get_text();
		proxy.passwd = m_nb_network_proxy_passwd_entry.get_text();
		proxy.resolve = m_nb_network_proxy_resolve_ckbutton.get_active();
	
		linqMain->setProxy(&proxy);
	}
	linqMain->connect(m_nb_network_server_host_combo.get_entry()->get_text().c_str(), atoi(m_nb_network_server_port_entry.get_text().c_str()));

	return NB_REGISTER_RESULT;
}

RegWizard::RegPage RegWizard::nb_register_result_next()
{
	return NB_NULL;
}

void RegWizard::on_next_clicked()
{
	if (isFinished) {
		delete this;
		linqMain->ShowMainWin();
		linqMain->main_win->changeStatus(STATUS_ONLINE);
		return;
	}

	RegPage next_page;
	switch (nb_current_page) {
		case NB_CHOOSE:
			next_page = nb_choose_next();
			break;
		case NB_BASIC_INFO:
			next_page = nb_basic_info_next();
			break;
		case NB_CONTACT_INFO:
			next_page = nb_contact_info_next();
			break;
		case NB_DETAIL_INFO:
			next_page = nb_detail_info_next();
			break;
		case NB_NETWORK_INFO:
			next_page = nb_network_info_next();
			break;
		case NB_REGISTER_RESULT:
			next_page = nb_register_result_next();
			break;
		default:
			next_page = NB_CHOOSE;
	}
	if (next_page == NB_NULL)
		return;
	m_notebook.set_current_page(next_page);
	nb_current_page = next_page;	
	m_back_button.set_sensitive(true);
}

void RegWizard::on_back_clicked()
{
	RegPage back_page;
	switch (nb_current_page) {
		case NB_BASIC_INFO:
			back_page = NB_CHOOSE;
			break;
		case NB_CONTACT_INFO:
			back_page = NB_BASIC_INFO;
			break;
		case NB_DETAIL_INFO:
			back_page = NB_CONTACT_INFO;
			break;
		case NB_NETWORK_INFO:
			if (m_nb_choose_new_user_radio.get_active())
				back_page = NB_DETAIL_INFO;
			else
				back_page = NB_CHOOSE;
			break;
		case NB_REGISTER_RESULT:
			back_page = NB_NETWORK_INFO;
			break;
		default:
			back_page = NB_CHOOSE;
	}
	m_notebook.set_current_page(back_page);
	nb_current_page = back_page;	
	if (nb_current_page==NB_CHOOSE)
		m_back_button.set_sensitive(false);
}

void RegWizard::on_cancel_clicked()
{
	delete this;
	linqMain->Quit();
}

void RegWizard::setFinished()
{
	m_next_button.set_label(_("Finish"));
	m_back_button.set_sensitive(false);
	isFinished = true;
}

void RegWizard::onConnect(bool connected)
{
	if (connected) {
		if (m_nb_choose_new_user_radio.get_active()) {
			m_nb_register_result_label.set_text(_("registering..."));
			Glib::ustring user = m_nb_basic_account_entry.get_text();
			Glib::ustring passwd = m_nb_basic_passwd_1_entry.get_text();
			linqMain->getUDPSession()->registerUser(user.c_str(), passwd.c_str());
		}
		else {			
			Glib::ustring user = m_nb_choose_user_entry.get_text();
			Glib::ustring passwd = m_nb_choose_password_entry.get_text();
			m_nb_register_result_label.set_text(_("Synching with server..."));			
			linqMain->login(user.c_str(), passwd.c_str(), STATUS_INVIS);
		}		
	}
	else
		m_nb_register_result_label.set_text(_("connect to server failed!"));
}

void RegWizard::onRegisterReply(uint8 error)
{
	if (nb_current_page == NB_REGISTER_RESULT) {
		switch (error) {
		case REG_SUCCESS:
		{
			m_nb_register_result_label.set_text(_("register success\nSynching with server..."));
			Glib::ustring user = m_nb_basic_account_entry.get_text();
			Glib::ustring passwd = m_nb_basic_passwd_1_entry.get_text();
			linqMain->login(user.c_str(), passwd.c_str(), STATUS_INVIS);
			break;
		}
		case REG_ERROR_UNKNOWN:
			m_nb_register_result_label.set_text(_("unknow error"));
			break;
		case REG_USER_EXISTS:
			m_nb_register_result_label.set_text(_("user already exists"));
			break;
		}		
	}
}

void RegWizard::onLoginReply(uint8 error)
{
	switch (error) {
	case LOGIN_SUCCESS:
	{
		Glib::ustring user;
		if (m_nb_choose_new_user_radio.get_active())
			user = m_nb_basic_account_entry.get_text();
		else
			user = m_nb_choose_user_entry.get_text();
		
		LinqUserList userList;
		userList.addUser(user.c_str());
				
		LinqDB::setNowUser(user.c_str(), true);
		
		getLinqOption(linqMain->options);
		LinqDB::saveOptions(linqMain->options);

		if (m_nb_choose_new_user_radio.get_active()) {
			m_nb_register_result_label.set_text(_("login succeeded\nclick finish to go on line."));			

			LinqUserInfo userInfo;
			getUserInfo(userInfo);
			LinqDB::saveUserInfo(userInfo);

			USER_INFO info;			
			info.name = userInfo.name.c_str();
			info.nick = userInfo.nick.c_str();
			info.gender = userInfo.gender;
			info.birth = userInfo.birth;
			info.email = userInfo.email.c_str();
			info.country = userInfo.country.c_str();
			info.city = userInfo.city.c_str();
			info.address = userInfo.address.c_str();
			info.postcode = userInfo.postcode.c_str();
			info.tel = userInfo.tel.c_str();
			info.mobile = userInfo.mobile.c_str();
			info.realname = userInfo.realname.c_str();
			info.occupation = userInfo.occupation.c_str();
			info.homepage = userInfo.homepage.c_str();
			info.intro = userInfo.intro.c_str();
			info.auth = userInfo.auth;			
						
			linqMain->getUDPSession()->updateUserInfo(info);
			
			linqMain->myInfo.name = userInfo.name;
			linqMain->myInfo.passwd = m_nb_basic_passwd_1_entry.get_text();
			linqMain->myInfo.now_status = STATUS_INVIS;
			setFinished();
		}
		else {
			m_nb_register_result_label.set_text(_("login succeeded\ngetting contact list.."));
			linqMain->myInfo.name = m_nb_choose_user_entry.get_text();
			linqMain->myInfo.passwd = m_nb_choose_password_entry.get_text();
			linqMain->myInfo.now_status = STATUS_INVIS;
			linqMain->getUDPSession()->getUserInfo();
			linqMain->getUDPSession()->getContactList();			
		}		
		break;
	}
	case LOGIN_WRONG_PASSWD:
		m_nb_register_result_label.set_text(_("wrong passwd."));
		break;
	case LOGIN_INVALID_USER:
		m_nb_register_result_label.set_text(_("no such user."));
		break;
	}
}

void RegWizard::getUserInfo(LinqUserInfo &info)
{
	info.name = m_nb_basic_account_entry.get_text();
	info.nick = m_nb_basic_nickname_entry.get_text();
	info.gender = m_nb_basic_gender_optionmenu.get_history();
			
	struct tm birth_tm;
	birth_tm.tm_sec = 0;
	birth_tm.tm_min = 0;
	birth_tm.tm_hour = 0;
	birth_tm.tm_mday = atoi(m_nb_basic_birth_day_entry.get_text().c_str());
	birth_tm.tm_mon = atoi(m_nb_basic_birth_month_entry.get_text().c_str()) -1;
	birth_tm.tm_year = atoi(m_nb_basic_birth_year_entry.get_text().c_str()) - 1900;
	birth_tm.tm_wday = 0;
	birth_tm.tm_yday = 0;
	birth_tm.tm_isdst = 0;
			
	info.birth = mktime(&birth_tm);
		
	info.email = m_nb_contact_email_entry.get_text();
	info.country = m_nb_basic_country_entry.get_text();
	info.city = m_nb_basic_city_entry.get_text();
	info.address = m_nb_contact_address_entry.get_text();
	info.postcode = m_nb_contact_postcode_entry.get_text();
	info.tel = m_nb_contact_tel_entry.get_text();
	info.mobile = m_nb_contact_mobile_entry.get_text();
	info.realname = m_nb_detail_realname_entry.get_text();
	info.occupation = m_nb_detail_occupation_combo.get_entry()->get_text();
	info.homepage = m_nb_detail_homepage_entry.get_text();
			
	info.intro = m_nb_detail_intro_textview.get_buffer()->get_text(false);
	
	if (m_nb_detail_auth_radio_1.get_active())
		info.auth = AUTH_ACCEPTED;
	else if (m_nb_detail_auth_radio_2.get_active())
		info.auth = AUTH_REQUEST;
	else
		info.auth = AUTH_REJECTED;
}

void RegWizard::getLinqOption(LinqOption &option)
{
	Glib::ustring passwd;	
	if (m_nb_choose_new_user_radio.get_active())
		passwd = m_nb_basic_passwd_1_entry.get_text();
	else
		passwd = m_nb_choose_password_entry.get_text();
	char hash[41];
	char str[51];		
	shahash_r(passwd.c_str(), hash);
	snprintf(str, 51, "%slinq", hash);
	shahash_r(str, hash);
	
	option.passwd_hash = hash;


	option.server_host = m_nb_network_server_host_combo.get_entry()->get_text();
	option.server_port = atoi(m_nb_network_server_port_entry.get_text().c_str());
	
	option.proxy_type = m_nb_network_proxy_type_optionmenu.get_history();
	option.proxy_host = m_nb_network_proxy_host_entry.get_text();
	option.proxy_port = atoi(m_nb_network_proxy_port_entry.get_text().c_str());
	option.proxy_username = m_nb_network_proxy_host_entry.get_text();
	option.proxy_passwd = m_nb_network_proxy_host_entry.get_text();
	option.proxy_resolve = m_nb_network_proxy_resolve_ckbutton.get_active();
}

void RegWizard::onUserInfoReply(USER_INFO &user)
{
	UserInfo_received = true;
			
	LinqUserInfo userInfo(user);
	LinqDB::saveUserInfo(userInfo);
	
	if (ContactList_received) {
		m_nb_register_result_label.set_text(_("got contactlist,click finish."));
		setFinished();
	}
}

void RegWizard::onContactListReply(const char *contacts[], int n)
{
	ContactList_received = true;
	
	//need to clear all contact?
	
	for (int i = 0; i < n; i++) {
		LinqContactInfo contact;
		contact.name = contacts[i];
		LinqDB::saveContactInfo(contact);
				
		linqMain->getUDPSession()->getContactInfo(contacts[i]);
	}	
	
	if (UserInfo_received) {
		m_nb_register_result_label.set_text(_("got contactlist,click finish."));
		setFinished();
	}
}
