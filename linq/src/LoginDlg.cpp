#include "config.h"
#include "LoginDlg.h"
#include "LinqMain.h"
#include "LinqDB.h"
#include "LinqContactInfo.h"
#include "sha.h"
#include "intl.h"

#include <gtkmm/separator.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/table.h>

LoginDlg::LoginDlg(std::list<std::string> &userList)
: LinqWindow(WIN_LOGIN),
  m_savepasswd_ckbutton(_("save password")),
  m_invislogin_ckbutton(_("login invisible"))
{
	set_border_width(2);
	set_default_size (100, 60);
	set_title(_("LinQ user login"));
	set_icon(linqMain->skin->linq.icon);
	set_position(Gtk::WIN_POS_CENTER);
	
	signal_delete_event().connect(SigC::slot(*this, &LoginDlg::on_delete_event));
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);
	
	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(2, 2, false));
	vbox->pack_start(*table, false, false, 0);
	
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("user name:")));
	table->attach(*label, 0, 1, 0, 1);

	m_user_combo.disable_activate();
	m_user_combo.get_entry()->set_editable(false);

	table->attach(m_user_combo, 1, 2, 0, 1);

	label = Gtk::manage(new Gtk::Label(_("user password:")));
	table->attach(*label, 0, 1, 1, 2);
	
	m_passwd_entry.set_visibility(false);
	m_passwd_entry.signal_activate().connect(SigC::slot(*this, &LoginDlg::on_login_clicked));

	table->attach(m_passwd_entry, 1, 2, 1, 2);
	
	m_passwd_entry.grab_focus();
	
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	vbox->pack_start(*hbox, false, false, 0);
	
	hbox->pack_start(m_savepasswd_ckbutton, true, true, 0);
	hbox->pack_start(m_invislogin_ckbutton, true, true, 0);
	
	//do this after passwd_entry is created.
	m_user_combo.get_entry()->signal_changed().connect(SigC::slot(*this, &LoginDlg::on_user_entry_changed));
	
	m_user_combo.set_popdown_strings(userList);	
	
	Gtk::HSeparator *hseparator;
	hseparator = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*hseparator, false, false, 3);
	
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	vbox->pack_start(*hbox, false, false, 0);
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button(_("Login")));
	button->signal_clicked().connect(SigC::slot(*this, &LoginDlg::on_login_clicked));
	hbox->pack_start(*button, true, false, 0);
	button = Gtk::manage(new Gtk::Button(_("Cancel")));
	button->signal_clicked().connect(SigC::slot(*this, &LoginDlg::on_cancel_clicked));
	hbox->pack_start(*button, true, false, 0);
	button = Gtk::manage(new Gtk::Button(_("Register wizard")));
	button->signal_clicked().connect(SigC::slot(*this, &LoginDlg::on_register_clicked));
	hbox->pack_start(*button, true, false, 0);
	
	show_all();
}

LoginDlg::~LoginDlg()
{
}


bool LoginDlg::on_delete_event(GdkEventAny* event)
{
	delete this;
	linqMain->Quit();
	return true;
}

void LoginDlg::on_user_entry_changed()
{
	Glib::ustring user;
	user = m_user_combo.get_entry()->get_text();
	if (!user.empty()) {
		LinqDB::setNowUser(user.c_str());
		
		if (LinqDB::loadOptions(options)) {		
			m_passwd_entry.set_text(options.saved_passwd);
			m_savepasswd_ckbutton.set_active(!(options.saved_passwd.empty()));
			m_invislogin_ckbutton.set_active(options.login_status == STATUS_INVIS);
		}
		else {
			m_passwd_entry.set_text("");
			m_savepasswd_ckbutton.set_active(false);
			m_invislogin_ckbutton.set_active(false);
		}
	}
}

void LoginDlg::on_login_clicked()
{
	Glib::ustring passwd;
	passwd = m_passwd_entry.get_text();
	if (passwd.empty()) {
		Gtk::MessageDialog dialog(*this, _("Please input the password!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
		dialog.run();
		return;
	}
	Glib::ustring user;
	user = m_user_combo.get_entry()->get_text();
	LinqUserInfo userInfo; //no use now, just do a test.
	if (!LinqDB::loadUserInfo(userInfo)) {
		Gtk::MessageDialog dialog(*this, _("The db currupted, use register ward to rebuild db?"), Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);			
		int result = dialog.run();
		if (result == Gtk::RESPONSE_YES) {			
			linqMain->ShowRegWizard(user.c_str(), passwd.c_str());
			delete this;
			return;
		}
	}
	if (!options.passwd_hash.empty()) {
		char hash[41];
		char str[51];
		
		shahash_r(passwd.c_str(), hash);
		snprintf(str, 51, "%slinq", hash);
		shahash_r(str, hash);
		if (options.passwd_hash != hash) {
			Gtk::MessageDialog dialog(*this, _("The password may be wrong, input again?"), Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);			
			int result = dialog.run();			
			if (result == Gtk::RESPONSE_YES)
				return;
		}
	}
	bool save;
	save = m_savepasswd_ckbutton.get_active();	
	uint32 status;
	if (m_invislogin_ckbutton.get_active())
		status = STATUS_INVIS;
	else
		status = STATUS_ONLINE;
	
	bool changed = false;
	
	if (options.login_status != status) {
		options.login_status = status;
		changed = true;
	}
	if (options.saved_passwd.empty() == save) {
		if (save)
			options.saved_passwd = passwd;
		else
			options.saved_passwd = "";
		changed = true;
	}
	if (changed)
		LinqDB::saveOptions(options);
	linqMain->options = options;
	
	linqMain->myInfo.name = user;
	linqMain->myInfo.passwd = passwd;
	linqMain->myInfo.login_status = status;	

	if (options.proxy_type == 0) {
		linqMain->setProxy(NULL);
	}
	else {
		PROXY_INFO proxy;
		if (options.proxy_type == 1)
			proxy.type = PROXY_SOCKS5;
		else
			proxy.type = PROXY_HTTPS;
		proxy.host = options.proxy_host;
		proxy.port = options.proxy_port;
		proxy.user = options.proxy_username;
		proxy.passwd = options.proxy_passwd;		
		proxy.resolve = options.proxy_resolve;		
	
		linqMain->setProxy(&proxy);
	}
	
	delete this;
	linqMain->ShowMainWin();
	linqMain->main_win->doLogin();
}

void LoginDlg::on_cancel_clicked()
{
	delete this;
	linqMain->Quit();
}

void LoginDlg::on_register_clicked()
{
	delete this;
	linqMain->ShowRegWizard();
}
