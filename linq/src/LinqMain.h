#ifndef __LINQ_MAIN_H__
#define __LINQ_MAIN_H__

#include "skin.h"
#include "MainWin.h"
#include "LinqWindow.h"
#include "LinqOption.h"
#include <linq/icqmain.h>


class LinqOnlineInfo {
public:
	LinqOnlineInfo(ONLINE_INFO &info);

	std::string name;
	uint32 status;
	uint16 tcp_ver;
	uint32 ip;
	uint32 real_ip;
	uint16 msg_port;
};

class LinqUser{
public:
	LinqUser();
	
	std::string name;
	std::string passwd;
	uint32 login_status;
	uint32 now_status;
};

class LinqMain   : public ICQMain {
public:	
	Skin *skin;
	LinqMain(SocketRegistry *reg);
	~LinqMain();

	void doLogin();
	void Quit();	

	void ShowLoginDlg(std::list<std::string> &userList);
	void ShowRegWizard(const char *user = NULL, const char *passwd = NULL);
	void ShowMainWin();

	virtual const char *getUserName();
	virtual TCPSessionListener *getTCPListener(const char *type, TCPSessionBase *session);
	virtual bool getContactInfo(ONLINE_INFO &info);

	virtual void onAck(uint32 seq);
	virtual void onSendError(uint16 seq);
	virtual void onConnect(bool connected);
	virtual void onRegisterReply(uint8 error);
	virtual void onLoginReply(uint8 error);
	virtual void onAddContactReply(const char *name, uint8 auth);
	virtual void onContactListReply(const char *contacts[], int n);
	virtual void onUserOnline(ONLINE_INFO &info);
	virtual void onUserOffline(const char *name);
	virtual void onUserStatus(const char *name, uint32 status);
	virtual void onSearchResult(SEARCH_RESULT result[], int n);
	virtual void onRecvMessage(ICQ_MSG &msg);
	virtual void onContactInfoReply(CONTACT_INFO &c);
	virtual void onUserInfoReply(USER_INFO &user);

	LinqWindow *findWindow(int type);
	
	std::list<LinqWindow *> windowList;	
	
	LinqUser myInfo;
	LinqOption options;
	MainWin *main_win;
private:
	std::list<LinqOnlineInfo> onlineInfoList;
	LinqOnlineInfo *findOnlineInfo(const char *name);

	gint send_keepalive_timeout;
	static gboolean send_keepalive_cb(gpointer data);
	
	gint check_send_queue_timeout;
	static gboolean check_send_queue_cb(gpointer data);

	void logLine(const char *fmt, ...);

	void destroyUser();
};

extern LinqMain *linqMain;

extern const char *status2text(uint32 status);

#endif
