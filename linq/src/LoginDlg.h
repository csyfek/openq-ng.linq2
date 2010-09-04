#ifndef __LINQ_LOGINDLG_H__
#define __LINQ_LOGINDLG_H__

#include "LinqWindow.h"
#include "LinqOption.h"

#include <gtkmm/window.h>
#include <gtkmm/combo.h>
#include <gtkmm/entry.h>
#include <gtkmm/checkbutton.h>

#include <list>
#include <string>

class LoginDlg  : public Gtk::Window, public LinqWindow{
public:
	LoginDlg(std::list<std::string> &userList);
	virtual ~LoginDlg();
protected:
	Gtk::Combo m_user_combo;
	Gtk::Entry m_passwd_entry;
	Gtk::CheckButton m_savepasswd_ckbutton;
	Gtk::CheckButton m_invislogin_ckbutton;
	
	virtual bool on_delete_event(GdkEventAny* event);
	virtual void on_user_entry_changed();
	virtual void on_login_clicked();
	virtual void on_cancel_clicked();
	virtual void on_register_clicked();

private:
	LinqOption options;
};

#endif
