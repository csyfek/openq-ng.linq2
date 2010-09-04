#ifndef __LINQ_SENDMSG_DLG_H__
#define __LINQ_SENDMSG_DLG_H__

#include "LinqWindow.h"
#include <time.h>

#include <gtkmm/window.h>
#include <gtkmm/notebook.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>
#include <gtkmm/button.h>
#include <gtkmm/paned.h>
#include <gtkmm/togglebutton.h>

#include <gtk/gtkwidget.h> // for imhtml...

#include <list>
#include <string>

class SendMsgDlgTab;

class SendMsgDlg : public Gtk::Window, public LinqWindow
{
friend class SendMsgDlgTab;
public:
	SendMsgDlg(const char *name, time_t when = 0, const char *text = NULL);
	virtual ~SendMsgDlg();

	void onRecvMessage(const char *name, time_t when, const char *text);
	void onSendMessage(const char *name); // User want to send msg to a buddy.

protected:
	Gtk::Notebook m_notebook;

	virtual bool on_window_delete_event(GdkEventAny * event);
private:	
	std::list<SendMsgDlgTab *> tabList;
};

class SendMsgDlgTab : public Gtk::VBox
{
public:
	SendMsgDlgTab(SendMsgDlg *oSendMsgDlg, const char *name);
	virtual ~SendMsgDlgTab();
		
	void doPresent();
	void doActive();
		
	void onRecvMessage(time_t when, const char *text);
	
	const std::string &getName() { return name; }
protected:
	Gtk::Label m_tab_label;
	
	Gtk::ToggleButton m_bold_button, m_italic_button, m_underline_button, m_bigfont_button, m_normalfont_button,
		  m_smallfont_button, m_selectfont_button, m_fgcolor_button, m_bgcolor_button, m_smilies_button;	
	Gtk::TextView m_input_textview;
	Gtk::Button m_send_button, m_warning_button, m_info_button;

	virtual void on_close_button_clicked();
	
	virtual void on_bold_button_toggled();
	virtual void on_italic_button_toggled();
	virtual void on_underline_button_toggled();
	virtual void on_bigfont_button_toggled();
	virtual void on_normalfont_button_toggled();
	virtual void on_smallfont_button_toggled();
	virtual void on_selectfont_button_toggled();
	virtual void on_fgcolor_button_toggled();
	virtual void on_bgcolor_button_toggled();
	virtual void on_smilies_button_toggled();
		
	virtual void on_send_button_clicked();
	virtual void on_warning_button_clicked();
	virtual void on_info_button_clicked();
	virtual bool on_input_textview_event(GdkEvent *e);	
private:
	SendMsgDlg *oSendMsgDlg;
	std::string name;

	GtkWidget *imhtml; // need to wrap it to C++.

	Gtk::VPaned *setup_im_pane();
	Gtk::VBox *build_conv_toolbutton_vbox();
	void setup_im_buttons(Gtk::HBox *hbox);

	void doSurround(const Glib::ustring &pre, const Glib::ustring &post);
	bool invertTags(const char *s1, const char *s2, bool really);
	void doAdvancePast(const Glib::ustring &pre, const Glib::ustring &post);
};

#endif
