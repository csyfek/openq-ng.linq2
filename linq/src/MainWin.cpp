#include "config.h"
#include "MainWin.h"
#include "LinqMain.h"
#include "SysMsgDlg.h"
#include "SysMsgListDlg.h"
#include "MsgManager.h"
#include "UserInfoDlg.h"
#include "SendMsgDlg.h"
#include "SearchDlg.h"
#include "PrefsDlg.h"
#include <linq/textstream.h>
#include "intl.h"

#include <gtkmm/button.h>
#include <gtkmm/main.h>

MainWin::MainWin()
: m_status_image(linqMain->skin->linq.status[STATUS_ONLINE]),
  m_status_label(_("Loging"))
{
	main_menu = NULL;
	status_menu = NULL;

	set_default_size (100, 300);
	set_title(_("LinQ"));
	set_icon(linqMain->skin->linq.icon);
	set_position (Gtk::WIN_POS_CENTER);
	signal_delete_event().connect(SigC::slot(*this, &MainWin::on_delete_event));
	signal_key_press_event().connect(SigC::slot(*this, &MainWin::on_key_press_release_event));
	signal_key_release_event().connect(SigC::slot(*this, &MainWin::on_key_press_release_event));

	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	vbox->show();
	add(*vbox);
	
	Gtk::Notebook *notebook;
	notebook = Gtk::manage(new Gtk::Notebook());
	notebook->show();
	vbox->pack_start(*notebook, true, true, 2);
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.buddy));
	image->show();
	notebook->append_page(contact_view, *image);
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.service));
	image->show();
	notebook->append_page(service_view, *image);
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.mobile));
	image->show();
	notebook->append_page(mobile_view, *image);
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.chain));
	image->show();
	notebook->append_page(chain_view, *image);

	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	vbox->pack_start(*hbox, false, false, 0);
	
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button());
	button->signal_clicked().connect(SigC::slot(*this, &MainWin::on_chatroom_button_clicked));	
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.chatroom));
	button->add(*image);
	hbox->pack_start(*button, false, false, 0);
	
	button = Gtk::manage(new Gtk::Button());
	button->signal_clicked().connect(SigC::slot(*this, &MainWin::on_search_button_clicked));	
	Gtk::HBox *hbox1;
	hbox1 = Gtk::manage(new Gtk::HBox(false, 0));
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.search));
	hbox1->pack_start(*image, false, false, 0);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Search")));
	hbox1->pack_start(*label, true, true, 0);
	button->add(*hbox1);
	hbox->pack_start(*button, true, true, 0);
	hbox->show_all();	
	
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	vbox->pack_start(*hbox, false, false, 2);
	button = Gtk::manage(new Gtk::Button());
	button->signal_clicked().connect(SigC::slot(*this, &MainWin::on_mozilla_button_clicked));
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.mozilla));
	button->add(*image);
	hbox->pack_start(*button, false, false, 0);
	button = Gtk::manage(new Gtk::Button());
	button->signal_clicked().connect(SigC::slot(*this, &MainWin::on_message_button_clicked));
	hbox1 = Gtk::manage(new Gtk::HBox(false, 0));
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.sysmsg));
	hbox1->pack_start(*image, false, false, 0);
	label = Gtk::manage(new Gtk::Label(_("Message")));
	hbox1->pack_start(*label, true, true, 0);
	button->add(*hbox1);
	hbox->pack_start(*button, true, true, 0);
	hbox->show_all();

	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	vbox->pack_start(*hbox, false, false, 0);
	button = Gtk::manage(new Gtk::Button());
	button->signal_clicked().connect(SigC::slot(*this, &MainWin::on_menu_button_clicked));
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.icon));
	button->add(*image);
	hbox->pack_start(*button, false, false, 0);
	button = Gtk::manage(new Gtk::Button());
	button->signal_clicked().connect(SigC::slot(*this, &MainWin::on_status_button_clicked));
	hbox1 = Gtk::manage(new Gtk::HBox(false, 0));
	
	hbox1->pack_start(m_status_image, false, false, 0);
	hbox1->pack_start(m_status_label, true, true, 0);
	button->add(*hbox1);
	hbox->pack_start(*button);
	hbox->show_all();
	
	show();
}

MainWin::~MainWin()
{
	if (status_animate_timeout_conn.connected())
		status_animate_timeout_conn.disconnect();
	if (main_menu)
		delete main_menu;
	if (status_menu)
		delete status_menu;
}

bool MainWin::on_delete_event(GdkEventAny *event)
{
	linqMain->Quit();
	return true;
}

bool MainWin::on_key_press_release_event(GdkEventKey *event)
{
	bool return_val=true;  //if return TRUE,the widget which in the main window will not receive any keyboard event.

	bool only_ctrl_pressed = ((event->state & GDK_CONTROL_MASK)&&(!(event->state & GDK_MOD1_MASK))&&(!(event->state & GDK_SHIFT_MASK)));
	//gboolean only_mod1_pressed = ((event->state & GDK_MOD1_MASK)&&(!(event->state & GDK_CONTROL_MASK))&&(!(event->state & GDK_SHIFT_MASK)));
	if (((event->keyval==GDK_q || event->keyval==GDK_Q) && only_ctrl_pressed)
	   ) {
		if (event->type==GDK_KEY_PRESS)
			linqMain->Quit();
	}
	else {
		return_val = false;
	}

	return return_val;
}

void MainWin::on_chatroom_button_clicked()
{
}

void MainWin::on_search_button_clicked()
{
	LinqWindow *win;
	win = linqMain->findWindow(WIN_SEARCH);
	if (win) {
		SearchDlg *dlg;
		dlg = (SearchDlg *)win;
		dlg->present();
	}
	else {
		new SearchDlg;
	}
}

void MainWin::on_mozilla_button_clicked()
{
}

void MainWin::on_message_button_clicked()
{
	LinqWindow *win;
	win = linqMain->findWindow(WIN_SYS_MESSAGE_LIST);
	if (win) {
		SysMsgListDlg *dlg;
		dlg = (SysMsgListDlg *)win;
		dlg->present();
	}
	else {
		new SysMsgListDlg;
	}
}

void MainWin::on_main_menu_netbookmark_activate()
{
}

void MainWin::on_main_menu_gourpfriend_activate()
{
}

void MainWin::on_main_menu_msghistory_activate()
{
	LinqWindow *win;
	win = linqMain->findWindow(WIN_MSG_MANAGER);
	if (win) {
		MsgManager *dlg;
		dlg = (MsgManager *)win;
		dlg->present();
	}
	else {
		new MsgManager;
	}
}

void MainWin::on_main_menu_memorandum_activate()
{
}

void MainWin::on_main_menu_mobilemsg_activate()
{
}

void MainWin::on_main_menu_changeskin_activate()
{
}

void MainWin::on_main_menu_changeuser_activate()
{
}

void MainWin::on_main_menu_regwizard_activate()
{
}

void MainWin::on_main_menu_myoption_activate()
{
	getContactInfo(linqMain->myInfo.name.c_str());
}

void MainWin::on_main_menu_sysoption_activate()
{
	LinqWindow *win;
	win = linqMain->findWindow(WIN_PREFS);
	if (win) {
		PrefsDlg *dlg;
		dlg = (PrefsDlg *)win;
		dlg->present();
	}
	else {
		new PrefsDlg;
	}
}

void MainWin::on_main_menu_help_activate()
{
}

void MainWin::on_main_menu_quit_activate()
{
	linqMain->Quit();
}

void MainWin::on_menu_button_clicked()
{
	if (main_menu == NULL) {	
		main_menu = Gtk::manage(new Gtk::Menu());
		
		Gtk::Menu::MenuList& menulist = main_menu->items();
		Gtk::Image *image;
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.netbookmark));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Unspecified"), *image, SigC::slot(*this, &MainWin::on_main_menu_netbookmark_activate)));

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.gourpfriend));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Group friend"), *image, SigC::slot(*this, &MainWin::on_main_menu_gourpfriend_activate)));

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.msghistory));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("msg log manage"), *image, SigC::slot(*this, &MainWin::on_main_menu_msghistory_activate)));

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.memorandum));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Memorandum"), *image, SigC::slot(*this, &MainWin::on_main_menu_memorandum_activate)));

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.mobilemsg));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Mobile message"), *image, SigC::slot(*this, &MainWin::on_main_menu_mobilemsg_activate)));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
		
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.changeskin));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Change Skin"), *image, SigC::slot(*this, &MainWin::on_main_menu_changeskin_activate)));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.changeuser));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Change User"), *image, SigC::slot(*this, &MainWin::on_main_menu_changeuser_activate)));

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.regwizard));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Register Wizard"), *image, SigC::slot(*this, &MainWin::on_main_menu_regwizard_activate)));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
		
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.myoption));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("My option"), *image, SigC::slot(*this, &MainWin::on_main_menu_myoption_activate)));
		
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.sysoption));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("System option"), *image, SigC::slot(*this, &MainWin::on_main_menu_sysoption_activate)));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
		
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.help));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Help"), *image, SigC::slot(*this, &MainWin::on_main_menu_help_activate)));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.quit));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Quit"), *image, SigC::slot(*this, &MainWin::on_main_menu_quit_activate)));

		main_menu->show_all();
	}
	
	main_menu->popup(1, gtk_get_current_event_time());
}

void MainWin::on_status_menu_online_activate()
{
	changeStatus(STATUS_ONLINE);
}

void MainWin::on_status_menu_invis_activate()
{
	changeStatus(STATUS_INVIS);
}

void MainWin::on_status_menu_away_activate()
{
	changeStatus(STATUS_AWAY);
}

void MainWin::on_status_menu_offline_activate()
{
	changeStatus(STATUS_OFFLINE);
}

void MainWin::on_status_button_clicked()
{
	if (status_menu == NULL) {	
		status_menu = Gtk::manage(new Gtk::Menu());
		
		Gtk::Menu::MenuList& menulist = status_menu->items();
		Gtk::Image *image;
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_ONLINE]));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Online"), *image, SigC::slot(*this, &MainWin::on_status_menu_online_activate)));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());


		Gtk::Menu *away_menu;
		away_menu = Gtk::manage(new Gtk::Menu());
		Gtk::Menu::MenuList& away_menulist = away_menu->items();
		
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_AWAY]));
		away_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("hi,i am away form computer..."), *image, SigC::slot(*this, &MainWin::on_status_menu_away_activate)));
		away_menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_AWAY]));
		away_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("working, please don't disturb me"), *image, SigC::slot(*this, &MainWin::on_status_menu_away_activate)));
		away_menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_AWAY]));
		away_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("i have go eating,wait..."), *image, SigC::slot(*this, &MainWin::on_status_menu_away_activate)));
		away_menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
		
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_OFFLINE]));
		away_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("No auto reply message..."), *image, SigC::slot(*this, &MainWin::on_status_menu_away_activate)));
		away_menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.main_menu.sysoption));
		away_menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Custom..."), *image, SigC::slot(*this, &MainWin::on_status_menu_away_activate)));
		away_menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());


		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_AWAY]));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Away"), *image, *away_menu));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
				
		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_INVIS]));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Invisible"), *image, SigC::slot(*this, &MainWin::on_status_menu_invis_activate)));
		menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());

		image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_OFFLINE]));
		menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Offline"), *image, SigC::slot(*this, &MainWin::on_status_menu_offline_activate)));

		status_menu->show_all();
	}
	
	status_menu->popup(1, gtk_get_current_event_time());
}

void MainWin::changeStatus(uint32 status)
{
	if (linqMain->myInfo.now_status != status) {
		if (linqMain->myInfo.now_status == STATUS_OFFLINE) {
			linqMain->myInfo.login_status = status;
			doLogin();
			return;
		}
		else if (status == STATUS_OFFLINE) {
			linqMain->logout();
			linqMain->myInfo.now_status = STATUS_OFFLINE;
		}
		else {
			linqMain->getUDPSession()->changeStatus(status);
			linqMain->myInfo.now_status = status;
		}
		m_status_image.set(linqMain->skin->linq.status[status]);
		m_status_label.set_text(status2text(status));
	}
	else {
		if (status == STATUS_OFFLINE) {
			if (status_animate_timeout_conn.connected()) {
				status_animate_timeout_conn.disconnect();
				m_status_image.set(linqMain->skin->linq.status[status]);
				m_status_label.set_text(status2text(status));
			}
		}
	}
}

void MainWin::getContactInfo(const gchar *name)
{
	std::list<LinqWindow *>::iterator iter;
	for (iter = linqMain->windowList.begin(); iter != linqMain->windowList.end(); ++iter) {
		LinqWindow *win = (LinqWindow *) *iter;
		if (win->type == WIN_VIEW_DETAIL) {
			UserInfoDlg *dlg = (UserInfoDlg *) *iter;
			if (dlg->getName() == name) {
				dlg->present();
				return;
			}
		}
	}
	
	// try to load from local db.
		
	if (g_ascii_strcasecmp(name, linqMain->myInfo.name.c_str())) {
		LinqContactInfo c;
		if (LinqDB::loadContactInfo(c)) {
			new UserInfoDlg(c);
		}
		else {
			std::list<std::string>::iterator iter;
			for (iter = ContactInfoQueryingList.begin(); iter != ContactInfoQueryingList.end(); ++iter) {
				if (*iter == name)
					return;
			}	
			ContactInfoQueryingList.push_front(name);

			linqMain->getUDPSession()->getContactInfo(name);
		}
	}
	else {
		LinqUserInfo user;
		if (LinqDB::loadUserInfo(user)) {
			new UserInfoDlg(user);
		}
		else
			linqMain->getUDPSession()->getUserInfo();
	}
}

bool MainWin::on_status_animate_timeout()
{
	static int i = 0;
	i++;
	if (i == 4)
		i = 0;
	m_status_image.set(linqMain->skin->linq.status[i]);
	return true;
}

void MainWin::doLogin()
{
	if (!status_animate_timeout_conn.connected())
		status_animate_timeout_conn = Glib::signal_timeout().connect(SigC::slot(*this, &MainWin::on_status_animate_timeout), STATUS_ANIMATE_TIMOUT_INTERVAL);		
	m_status_label.set_text(_("Loging"));
	linqMain->connect(linqMain->options.server_host.c_str(), linqMain->options.server_port);	
}

void MainWin::onConnect(bool connected)
{
	if (connected) {
		linqMain->login(linqMain->myInfo.name.c_str(), linqMain->myInfo.passwd.c_str(), linqMain->myInfo.login_status);
	}
	else {
		if (status_animate_timeout_conn.connected()) {
			status_animate_timeout_conn.disconnect();
			m_status_image.set(linqMain->skin->linq.status[STATUS_OFFLINE]);
		}
	}		
}

void MainWin::onLoginReply(uint8 error)
{
	if (status_animate_timeout_conn.connected()) {
		status_animate_timeout_conn.disconnect();
	}

	switch (error) {
	case LOGIN_SUCCESS:
		linqMain->myInfo.now_status = linqMain->myInfo.login_status;	
		break;
	case LOGIN_WRONG_PASSWD:
		linqMain->myInfo.now_status = STATUS_OFFLINE;
		break;
	case LOGIN_INVALID_USER:
		linqMain->myInfo.now_status = STATUS_OFFLINE;
		break;
	}
	m_status_image.set(linqMain->skin->linq.status[linqMain->myInfo.now_status]);
	m_status_label.set_text(status2text(linqMain->myInfo.now_status));
}

void MainWin::onUserOnline(ONLINE_INFO &info)
{
	contact_view.onUserOnline(info.name);
}

void MainWin::onUserOffline(const char *name)
{
	contact_view.onUserOffline(name);
}

void MainWin::onUserStatus(const char *name, uint32 status)
{
	g_print("%s changed status to %s.\n", name, status2text(status));
	contact_view.onUserStatus(name, status);
}

void MainWin::onAddContactReply(const char *name, uint8 auth)
{
	LinqWindow *win;
	std::list<LinqWindow *>::iterator iter;
	for (iter = linqMain->windowList.begin(); iter != linqMain->windowList.end(); ++iter) {
		win = (LinqWindow *) *iter;
		if (win->type == WIN_SYS_MESSAGE) {
			SysMsgDlg *dlg = (SysMsgDlg *) *iter;
			if (dlg->onAddContactReply(name, auth))
				return;
		}
	}

	new SysMsgDlg(name, auth);
}

void MainWin::onRecvMessage(ICQ_MSG &msg)
{	
	tm *t = localtime(&(msg.when));
	g_print("received message from %s(%s):", msg.from, t ? asctime(t) : "NULL");

	if (msg.type == MSG_TEXT)
	{
		LinqWindow *win;
		win = linqMain->findWindow(WIN_SEND_MESSAGE);		
		if (win) {
			SendMsgDlg *dlg;
			dlg = (SendMsgDlg *)win;
			dlg->onRecvMessage(msg.from, msg.when, msg.text);
		}
		else {
			new SendMsgDlg(msg.from, msg.when, msg.text);
		}	
	}
	else if ((msg.type == MSG_AUTH_ACCEPTED)||(msg.type == MSG_AUTH_REJECTED)||(msg.type == MSG_AUTH_REQUEST)||(msg.type == MSG_ADDED)||(msg.type == MSG_ANNOUNCE))
	{
		new SysMsgDlg(msg.type, msg.from, msg.when, msg.text);
	}
	else if (msg.type == MSG_TCP_REQUEST) {
		TextInStream in(msg.text);
		string request_type;

		in >> request_type;
		g_print("%s request you to accept %s", msg.from, request_type.c_str());

		linqMain->acceptTCPRequest(request_type.c_str(), msg.from, 0);
	}
}

void MainWin::onContactInfoReply(CONTACT_INFO &c)
{	
	contact_view.onContactInfoReply(c);
	
	//only show the dialog when it is explicit queried by user.
	std::list<std::string>::iterator contact_iter;
	for (contact_iter = ContactInfoQueryingList.begin(); contact_iter != ContactInfoQueryingList.end(); ++contact_iter) {
		if (*contact_iter == c.name) {
			LinqContactInfo contact(c);
			new UserInfoDlg(contact);
			return;
		}
	}

	std::list<LinqWindow *>::iterator iter;
	for (iter = linqMain->windowList.begin(); iter != linqMain->windowList.end(); ++iter) {
		LinqWindow *win = (LinqWindow *) *iter;
		if (win->type == WIN_VIEW_DETAIL) {
			UserInfoDlg *dlg = (UserInfoDlg *) *iter;
			if (dlg->getName() == c.name) {
				LinqContactInfo contact(c);
				dlg->onContactInfoReply(contact); //update info.
				return;
			}
		}
	}
}

void MainWin::onUserInfoReply(USER_INFO &user)
{
	std::list<LinqWindow *>::iterator iter;
	for (iter = linqMain->windowList.begin(); iter != linqMain->windowList.end(); ++iter) {
		LinqWindow *win = (LinqWindow *) *iter;
		if (win->type == WIN_VIEW_DETAIL) {
			UserInfoDlg *dlg = (UserInfoDlg *) *iter;
			if (dlg->getName() == linqMain->myInfo.name) {
				LinqUserInfo userInfo(user);
				dlg->onUserInfoReply(userInfo); //update
				return;
			}
		}
	}
	
	LinqUserInfo userInfo(user);
	new UserInfoDlg(userInfo);
}
