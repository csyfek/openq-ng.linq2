#ifndef __LINQ_MAIN_WIN_H__
#define __LINQ_MAIN_WIN_H__

#include <linq/udpsession.h>
#include "MainWin_ContactView.h"
#include "MainWin_ServiceView.h"
#include "MainWin_MobileView.h"
#include "MainWin_ChainView.h"

#include <gtkmm/window.h>
#include <gtkmm/menu.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

const int STATUS_ANIMATE_TIMOUT_INTERVAL = 500;

class MainWin : public Gtk::Window
{
public:
	MainWin();
	virtual ~MainWin();

	void onConnect(bool connected);
	void onLoginReply(uint8 error);
	void onUserOnline(ONLINE_INFO &info);
	void onUserOffline(const char *name);
	void onUserStatus(const char *name, uint32 status);
	void onAddContactReply(const char *name, uint8 auth);
	void onRecvMessage(ICQ_MSG &msg);
	void onContactInfoReply(CONTACT_INFO &c);
	void onUserInfoReply(USER_INFO &user);

	void changeStatus(uint32 status);
	void getContactInfo(const gchar *name);
	void doLogin();
protected:
	Gtk::Menu *main_menu;
	Gtk::Menu *status_menu;

	Gtk::Image m_status_image;
	Gtk::Label m_status_label;

	virtual bool on_delete_event(GdkEventAny *event);
	virtual bool on_key_press_release_event(GdkEventKey *event);
	
	virtual void on_chatroom_button_clicked();
	virtual void on_search_button_clicked();
	virtual void on_mozilla_button_clicked();
	virtual void on_message_button_clicked();
	virtual void on_menu_button_clicked();
	virtual void on_status_button_clicked();
	
	virtual void on_main_menu_netbookmark_activate();
	virtual void on_main_menu_gourpfriend_activate();
	virtual void on_main_menu_msghistory_activate();
	virtual void on_main_menu_memorandum_activate();
	virtual void on_main_menu_mobilemsg_activate();
	virtual void on_main_menu_changeskin_activate();
	virtual void on_main_menu_changeuser_activate();
	virtual void on_main_menu_regwizard_activate();
	virtual void on_main_menu_myoption_activate();
	virtual void on_main_menu_sysoption_activate();
	virtual void on_main_menu_help_activate();
	virtual void on_main_menu_quit_activate();
	
	virtual void on_status_menu_online_activate();
	virtual void on_status_menu_away_activate();
	virtual void on_status_menu_invis_activate();
	virtual void on_status_menu_offline_activate();

	SigC::Connection status_animate_timeout_conn;
	virtual bool on_status_animate_timeout();	
private:
	ContactView contact_view;
	ServiceView service_view;
	MobileView mobile_view;
	ChainView chain_view;

	std::list<std::string> ContactInfoQueryingList;
};

#endif
