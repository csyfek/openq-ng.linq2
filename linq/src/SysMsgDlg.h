#ifndef __LINQ_SYSMSG_DLG_H__
#define __LINQ_SYSMSG_DLG_H__

#include "LinqWindow.h"

#include <time.h>
#include <gtkmm/window.h>
#include <gtkmm/textview.h>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>

class SysMsgDlg : public Gtk::Window, public LinqWindow
{
public:
	SysMsgDlg(uint8 type, const char *from, time_t when, const char *text); // onRecvMessage
	SysMsgDlg(const char *name, uint8 auth); // onAddContactReply
	virtual ~SysMsgDlg();
	bool onAddContactReply(const char *name, uint8 auth);
protected:
	Gtk::VBox m_vbox;
	Gtk::Entry m_from_entry;
	Gtk::TextView m_textview;
	Gtk::HBox m_button_hbox;
	Gtk::Button *accept_button, *reject_button, *addFriend_button;
	Gtk::VBox *sendReject_vbox;
	Gtk::TextView *sendReject_textview;
	Gtk::VBox *sendRequest_vbox;
	Gtk::TextView *sendRequest_textview;

	virtual bool on_window_delete_event(GdkEventAny* event);
	virtual void on_detailInfo_button_clicked();
	virtual void on_accept_button_clicked();
	virtual void on_reject_button_clicked();
	virtual void on_addFriend_button_clicked();
	virtual void on_close_button_clicked();
	virtual void on_sendRequest_button_clicked();
	virtual void on_sendReject_button_clicked();
private:
	bool waitingAddContactReply;
	void createWindow();
	void setMsg(uint8 type, const char *from, time_t when, const char *text);
	void setMsg(const char *name, uint8 auth);

};

#endif
