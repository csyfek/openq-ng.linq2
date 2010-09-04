#include "config.h"
#include "LinqMain.h"
#include "LinQ.h"
#include "LinqContactInfo.h"
#include "LinqGlobalSetting.h"
#include "LinqUserList.h"
#include "LinqMsg.h"
#include "LoginDlg.h"
#include "RegWizard.h"
#include "SearchDlg.h"
#include "SendFileDlg.h"
#include "LinqDB.h"
#include "intl.h"


#include <gtkmm/main.h>

const char *status2text(uint32 status)
{
	switch (status) {
	case STATUS_ONLINE: return _("Online");
	case STATUS_AWAY: return _("Away");
	case STATUS_INVIS: return _("Invisible");
	case STATUS_OFFLINE: return _("Offline");
	}

	return _("Unknown");
}

LinqMain *linqMain;


LinqOnlineInfo::LinqOnlineInfo(ONLINE_INFO &info)
{
	name = info.name;
	status = info.status;
	tcp_ver = info.tcp_ver;
	ip = info.ip;
	real_ip = info.real_ip;
	msg_port = info.msg_port;
}

LinqUser::LinqUser()
{
	login_status = STATUS_ONLINE;
	now_status = STATUS_OFFLINE;
}

LinqMain::LinqMain(SocketRegistry *reg):ICQMain(reg)
{
	linqMain = this;
	main_win = NULL;
	send_keepalive_timeout = 0;
	check_send_queue_timeout = 0;	
}

LinqMain::~LinqMain()
{
	linqMain = NULL;
}

void LinqMain::doLogin()
{
	skin = new Skin;
	
	LinqDB::init();
	
	LinqGlobalSetting *setting;
	setting = new LinqGlobalSetting;
	LinqDB::loadGlobalSetting(*setting);
	if (setting->use_custom_font) {
		if (!setting->custom_font.empty()) {
			std::string aa;
			aa = "style \"custom-font\" { font_name= \"";
			aa += setting->custom_font;
			aa += "\" }\nclass \"GtkWidget\" style \"custom-font\"\n";
			gtk_rc_parse_string(aa.c_str());
		}
	}
	delete setting;

	LinqUserList *oLinqUserList;
	oLinqUserList = new LinqUserList;
	oLinqUserList->getAllUsers();
		
	
	if (oLinqUserList->userList.empty())
		ShowRegWizard();
	else
		ShowLoginDlg(oLinqUserList->userList);
	delete oLinqUserList;
	
	Gtk::Main::run();
}

void LinqMain::Quit()
{	
	destroyUser();
	
	if (main_win)
		delete main_win;
	delete skin;
	Gtk::Main::quit();
}

void LinqMain::ShowLoginDlg(std::list<std::string> &userList)
{
	LinqWindow *win;
	win = findWindow(WIN_LOGIN);	
	if (win) {
		LoginDlg *dlg;
		dlg = (LoginDlg *)win;
		dlg->present();
	}
	else {
		new LoginDlg(userList);
	}
}

void LinqMain::ShowRegWizard(const char *user, const char *passwd)
{
	LinqWindow *win;
	win = findWindow(WIN_REG_WIZARD);	
	if (win) {
		RegWizard *dlg;
		dlg = (RegWizard *)win;
		dlg->present();
	}
	else {
		new RegWizard(user, passwd);
	}
}

void LinqMain::ShowMainWin()
{
	if (!main_win)
		main_win = new MainWin;
	else
		main_win->present();
}

LinqWindow *LinqMain::findWindow(int type)
{
	std::list<LinqWindow *>::iterator iter;
	for (iter = windowList.begin(); iter != windowList.end(); ++iter) {
		LinqWindow *win = (LinqWindow *) *iter;
		if (win->type == type)
			return win;
	}
	return NULL;
}

void LinqMain::destroyUser()
{
	if (myInfo.now_status != STATUS_OFFLINE)
		logout();
	while (!windowList.empty())
		delete (LinqWindow *) windowList.front(); //it will remove from windowList when be deleted.
	
	onlineInfoList.clear();
}

void LinqMain::logLine(const char *fmt, ...)
{
/*	char buf[1024];

	va_list args;
	va_start(args, fmt);

	vsnprintf(buf, sizeof(buf), fmt, args);

	va_end(args);
	g_print("%s",buf);*/
}

const char *LinqMain::getUserName()
{
	return myInfo.name.c_str();
}

bool LinqMain::getContactInfo(ONLINE_INFO &info)
{
	LinqOnlineInfo *c = findOnlineInfo(info.name);
	if (!c)
		return false;

	info.status = c->status;
	info.tcp_ver = c->tcp_ver;
	info.ip = c->ip;
	info.real_ip = c->real_ip;
	info.msg_port = c->msg_port;
	return true;
}

TCPSessionListener *LinqMain::getTCPListener(const char *type, TCPSessionBase *session)
{
	TCPSessionListener *l = ICQMain::getTCPListener(type, session);
	if (!l) {
		if (strcmp(type, "SENDFILE") == 0)
			l = new CSendFileDlg(session);
	}
	return l;
}

LinqOnlineInfo *LinqMain::findOnlineInfo(const char *name)
{
	std::list<LinqOnlineInfo>::iterator iter;
	for (iter = onlineInfoList.begin(); iter!= onlineInfoList.end(); ++iter) {
		LinqOnlineInfo *c = &(*iter);
		if (c->name == name)
			return c;
	}
	return NULL;
}

gboolean LinqMain::send_keepalive_cb(gpointer data)
{
	linqMain->getUDPSession()->sendKeepAlive();
	return true;
}

gboolean LinqMain::check_send_queue_cb(gpointer data)
{
	linqMain->getUDPSession()->checkSendQueue();
	return true;
}

void LinqMain::onAck(uint32 seq)
{
	logLine("packet %u is acked", seq);
}

void LinqMain::onSendError(uint16 seq)
{
	logLine("packet %d failed", seq);
}

void LinqMain::onConnect(bool connected)
{
	if (connected) {
		check_send_queue_timeout = gtk_timeout_add(SEND_TIMEOUT * 1000, check_send_queue_cb, this);		
	}
	else {
		if (check_send_queue_timeout) {
			gtk_timeout_remove(check_send_queue_timeout);
			check_send_queue_timeout = 0;
		}
		if (send_keepalive_timeout) {
			gtk_timeout_remove(send_keepalive_timeout);
			send_keepalive_timeout = 0;
		}
	}		

	LinqWindow *win;
	win = linqMain->findWindow(WIN_REG_WIZARD);	
	if (win) {
		RegWizard *dlg;
		dlg = (RegWizard *)win;
		dlg->onConnect(connected);
	}
	if (main_win)
		main_win->onConnect(connected);
}

void LinqMain::onRegisterReply(uint8 error)
{
	LinqWindow *win;
	win = linqMain->findWindow(WIN_REG_WIZARD);	
	if (win) {
		RegWizard *dlg;
		dlg = (RegWizard *)win;
		dlg->onRegisterReply(error);
	}
}

void LinqMain::onLoginReply(uint8 error)
{
	if (error == LOGIN_SUCCESS) {
		send_keepalive_timeout = gtk_timeout_add(KEEPALIVE_TIMEOUT * 1000, send_keepalive_cb, this);
	}

	LinqWindow *win;
	win = linqMain->findWindow(WIN_REG_WIZARD);	
	if (win) {
		RegWizard *dlg;
		dlg = (RegWizard *)win;
		dlg->onLoginReply(error);
	}
	if (main_win)
		main_win->onLoginReply(error);
}

void LinqMain::onAddContactReply(const char *name, uint8 auth)
{
	LinqWindow *win;
	win = findWindow(WIN_SEARCH);
	if (win) {
		SearchDlg *dlg;
		dlg = (SearchDlg *)win;
		if (dlg->onAddContactReply(name, auth))
			return;
	}
	if (main_win)
		main_win->onAddContactReply(name, auth);	
}

void LinqMain::onContactListReply(const char *contacts[], int n)
{
	LinqWindow *win;
	win = linqMain->findWindow(WIN_REG_WIZARD);
	if (win) {
		RegWizard *dlg;
		dlg = (RegWizard *)win;
		dlg->onContactListReply(contacts, n);
	}
}

void LinqMain::onUserOnline(ONLINE_INFO &info)
{
	LinqOnlineInfo *c;
	std::list<LinqOnlineInfo>::iterator iter;
	for (iter = onlineInfoList.begin(); iter!= onlineInfoList.end(); ++iter) {
		c = &(*iter);
		if (c->name == info.name) {
			onlineInfoList.erase(iter);
			break;
		}
	}
	onlineInfoList.push_front(info);

	if (main_win)
		main_win->onUserOnline(info);
}

void LinqMain::onUserOffline(const char *name)
{
	std::list<LinqOnlineInfo>::iterator iter;
	for (iter = onlineInfoList.begin(); iter!= onlineInfoList.end(); ++iter) {
		LinqOnlineInfo *c = &(*iter);
		if (c->name == name) {
			onlineInfoList.erase(iter);
			break;
		}
	}	

	if (main_win)
		main_win->onUserOffline(name);
}

void LinqMain::onUserStatus(const char *name, uint32 status)
{
	if (main_win)
		main_win->onUserStatus(name, status);
}

void LinqMain::onSearchResult(SEARCH_RESULT result[], int n)
{
	LinqWindow *win;
	win = linqMain->findWindow(WIN_SEARCH);
	if (win) {
		SearchDlg *dlg;
		dlg = (SearchDlg *)win;
		dlg->onSearchResult(result, n);
	}
}

void LinqMain::onRecvMessage(ICQ_MSG &msg)
{
	ICQMain::onRecvMessage(msg);
	
	LinqMsg linqMsg(msg);
	LinqDB::saveMsg(linqMsg);

	if (main_win)
		main_win->onRecvMessage(msg);
}

void LinqMain::onContactInfoReply(CONTACT_INFO &c)
{
	LinqContactInfo contact(c);
	LinqDB::saveContactInfo(contact);

	if (main_win)
		main_win->onContactInfoReply(c);
}

void LinqMain::onUserInfoReply(USER_INFO &user)
{
	LinqUserInfo contact(user);
	LinqDB::saveUserInfo(contact);

	LinqWindow *win;
	win = linqMain->findWindow(WIN_REG_WIZARD);	
	if (win) {
		RegWizard *dlg;
		dlg = (RegWizard *)win;
		dlg->onUserInfoReply(user);
	}

	if (main_win)
		main_win->onUserInfoReply(user);
}
