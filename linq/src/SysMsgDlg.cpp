#include "config.h"
#include "SysMsgDlg.h"
#include "LinqMain.h"
#include "intl.h"

#include <gtkmm/frame.h>

SysMsgDlg::SysMsgDlg(uint8 type, const char *from, time_t when, const char *text) : LinqWindow(WIN_SYS_MESSAGE)
{
	waitingAddContactReply = false;
	accept_button = NULL;
	reject_button = NULL;
	addFriend_button = NULL;
	sendRequest_vbox = NULL;
	sendReject_vbox = NULL;
	createWindow();
	setMsg(type, from, when, text);
	show_all();
}

SysMsgDlg::SysMsgDlg(const char *name, uint8 auth) : LinqWindow(WIN_SYS_MESSAGE)
{
	waitingAddContactReply = false;
	accept_button = NULL;
	reject_button = NULL;
	addFriend_button = NULL;
	sendRequest_vbox = NULL;
	sendReject_vbox = NULL;
	createWindow();
	setMsg(name, auth);
	show_all();
}

SysMsgDlg::~SysMsgDlg()
{
}

void SysMsgDlg::createWindow()
{
	signal_delete_event().connect(SigC::slot(*this, &SysMsgDlg::on_window_delete_event));
	set_title(_("LinQ - Sys message"));

	add(m_vbox);

	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Come from")));
	m_vbox.pack_start(*frame);

	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	frame->add(*hbox);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("LinQ#: ")));
	hbox->pack_start(*label, false, false, 0);
	hbox->pack_start(m_from_entry);
	label = Gtk::manage(new Gtk::Label(_("Nick name:")));
	hbox->pack_start(*label);
	Gtk::Entry *entry;
	entry = Gtk::manage(new Gtk::Entry());
	hbox->pack_start(*entry);
	label = Gtk::manage(new Gtk::Label(_("Detail info->")));
	hbox->pack_start(*label);
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button());
	hbox->pack_start(*button);
	button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_detailInfo_button_clicked));
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.face[0]));
	button->add(*image);

	label = Gtk::manage(new Gtk::Label(_("Message content")));
	m_vbox.pack_start(*label);

	frame = Gtk::manage(new Gtk::Frame());
	m_vbox.pack_start(*frame);
	frame->set_shadow_type(Gtk::SHADOW_IN);
	
	m_textview.set_wrap_mode(Gtk::WRAP_WORD);
	m_textview.set_size_request(-1, 100);
	frame->add(m_textview);

	m_vbox.pack_start(m_button_hbox);
	
	button = Gtk::manage(new Gtk::Button(_("Close")));	
	m_button_hbox.pack_end(*button, false, false, 0);
	button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_close_button_clicked));
}

void SysMsgDlg::setMsg(uint8 type, const char *from, time_t when, const char *text)
{
	m_from_entry.set_text(from);
	gchar *msg = NULL;
	if (type == MSG_AUTH_ACCEPTED) {
		if (text[0])
			msg = g_strdup_printf("User %s have accepted your authorization request.\n\nAppend msg:%s", from, text);
		else
			msg = g_strdup_printf("User %s have accepted your authorization request.", from);
	}
	else if (type == MSG_AUTH_REJECTED) {
		if (text[0])
			msg = g_strdup_printf("User %s have rejected your authorization request.\n\nAppend msg:%s", from, text);
		else
			msg = g_strdup_printf("User %s have rejected your authorization request.", from);

		addFriend_button = Gtk::manage(new Gtk::Button(_("add friend")));
		m_button_hbox.pack_end(*addFriend_button, false, false, 0);
		addFriend_button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_addFriend_button_clicked));
	}
	else if (type == MSG_AUTH_REQUEST) {
		if (text[0])
			msg = g_strdup_printf("User %s request for your authorization.\n\nAppend msg:\n%s", from, text);
		else
			msg = g_strdup_printf("User %s request for your authorization.", from);
		accept_button = Gtk::manage(new Gtk::Button(_("accept")));
		m_button_hbox.pack_start(*accept_button, false, false, 0);
		accept_button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_accept_button_clicked));
		reject_button = Gtk::manage(new Gtk::Button(_("reject")));
		m_button_hbox.pack_start(*reject_button, false, false, 0);
		reject_button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_reject_button_clicked));
		addFriend_button = Gtk::manage(new Gtk::Button(_("accept and add friend")));
		m_button_hbox.pack_end(*addFriend_button, false, false, 0);
		addFriend_button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_accept_button_clicked)); //if use click add friend, accept him too.
		addFriend_button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_addFriend_button_clicked));
	}
	else if (type == MSG_ADDED) {
		msg = g_strdup_printf("User %s have add you to his/her contact list.", from);
		//need to check whether already added...
		Gtk::Button *button;
		button = Gtk::manage(new Gtk::Button(_("add friend")));
		m_button_hbox.pack_end(*button, false, false, 0);
		button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_addFriend_button_clicked));
	}
	else if (type == MSG_ANNOUNCE) {
		msg = g_strdup_printf("System announce:\n%s", text);
	}
	m_textview.get_buffer()->set_text(msg);
	g_free(msg);	
}

void SysMsgDlg::setMsg(const char *name, uint8 auth)
{
	m_from_entry.set_text(name);
	gchar *msg = NULL;	
	switch (auth) {
	case AUTH_ACCEPTED:
		msg = g_strdup_printf("%s accepted your request", name);
		break;
	case AUTH_REJECTED:
		msg = g_strdup_printf("%s rejected your request", name);
		addFriend_button = Gtk::manage(new Gtk::Button(_("add friend")));
		m_button_hbox.pack_end(*addFriend_button);
		addFriend_button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_addFriend_button_clicked));		
		break;
	case AUTH_REQUEST:
		msg = g_strdup_printf("%s require your request", name);
		
		sendRequest_vbox = Gtk::manage(new Gtk::VBox(false, 0));
		m_vbox.pack_start(*sendRequest_vbox);
		sendRequest_textview = Gtk::manage(new Gtk::TextView());
		sendRequest_textview->set_size_request(-1, 50);
		sendRequest_vbox->pack_start(*sendRequest_textview, false, false, 0);
		Gtk::Button *button;
		button = Gtk::manage(new Gtk::Button(_("Send request")));
		sendRequest_vbox->pack_end(*button, false, false, 0);
		button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_sendRequest_button_clicked));		
		break;
	}
	m_textview.get_buffer()->set_text(msg);
	g_free(msg);
}

bool SysMsgDlg::on_window_delete_event(GdkEventAny * event)
{
	delete this;
	return true;
}

void SysMsgDlg::on_detailInfo_button_clicked()
{
	linqMain->main_win->getContactInfo(m_from_entry.get_text().c_str());
}

void SysMsgDlg::on_accept_button_clicked()
{
	linqMain->getUDPSession()->sendMessage(MSG_AUTH_ACCEPTED, m_from_entry.get_text().c_str(), "");
	m_textview.get_buffer()->set_text("accept info have send out, please return.");
}

void SysMsgDlg::on_reject_button_clicked()
{
	if (!sendReject_vbox) {
		sendReject_vbox = Gtk::manage(new Gtk::VBox(false, 0));
		m_vbox.pack_start(*sendReject_vbox);
		sendReject_textview = Gtk::manage(new Gtk::TextView());
		sendReject_textview->set_size_request(-1, 50);
		sendReject_textview->get_buffer()->set_text("You are not welcomed.");
		Gtk::Frame *frame = Gtk::manage(new Gtk::Frame());		
		frame->set_shadow_type(Gtk::SHADOW_IN);
		frame->add(*sendReject_textview);
		sendReject_vbox->pack_start(*frame);
		Gtk::Button *button;
		button = Gtk::manage(new Gtk::Button(_("Send reject msg")));
		sendReject_vbox->pack_end(*button, false, false, 0);
		button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_sendReject_button_clicked));
		sendReject_vbox->show_all();
	}
	sendReject_textview->grab_focus();
}

void SysMsgDlg::on_addFriend_button_clicked()
{
	waitingAddContactReply = true;
	linqMain->getUDPSession()->addContact(m_from_entry.get_text().c_str());
	m_textview.get_buffer()->set_text("add friend request have send out, please wait.");	
	
	if (sendReject_vbox) {
		delete (sendReject_vbox);
		sendReject_vbox = NULL;
	}
}

void SysMsgDlg::on_sendRequest_button_clicked()
{
	linqMain->getUDPSession()->sendMessage(MSG_AUTH_REQUEST, m_from_entry.get_text().c_str(), sendRequest_textview->get_buffer()->get_text(false).c_str());
	delete this;
}

void SysMsgDlg::on_sendReject_button_clicked()
{
	linqMain->getUDPSession()->sendMessage(MSG_AUTH_REJECTED, m_from_entry.get_text().c_str(), sendReject_textview->get_buffer()->get_text(false).c_str());
	delete this;
}

void SysMsgDlg::on_close_button_clicked()
{
	delete this;
}

bool SysMsgDlg::onAddContactReply(const char *name, uint8 auth)
{
	if (waitingAddContactReply && (m_from_entry.get_text() == name)) {
		waitingAddContactReply = false;
		
		if (accept_button) {
			delete (accept_button);
			accept_button = NULL;
		}
		if (reject_button) {
			delete (reject_button);
			reject_button = NULL;
		}
		if (addFriend_button) {
			delete (addFriend_button);
			addFriend_button = NULL;
		}
		if (sendReject_vbox) {
			delete (sendReject_vbox);
			sendReject_vbox = NULL;
		}
		
		gchar *msg = NULL;	
		switch (auth) {
		case AUTH_ACCEPTED:
			msg = g_strdup_printf("%s accepted your request", name);
			break;
		case AUTH_REJECTED:
			msg = g_strdup_printf("%s rejected your request", name);
			break;
		case AUTH_REQUEST:
			msg = g_strdup_printf("%s require your request", name);
		
			if (!sendRequest_vbox) {
				sendRequest_vbox = Gtk::manage(new Gtk::VBox(false, 0));
				m_vbox.pack_start(*sendRequest_vbox);
				sendRequest_textview = Gtk::manage(new Gtk::TextView());
				sendRequest_textview->set_size_request(-1, 50);
				Gtk::Frame *frame = Gtk::manage(new Gtk::Frame());		
				frame->set_shadow_type(Gtk::SHADOW_IN);
				frame->add(*sendRequest_textview);
				sendRequest_vbox->pack_start(*frame);
				Gtk::Button *button;
				button = Gtk::manage(new Gtk::Button(_("Send request")));
				sendRequest_vbox->pack_end(*button, false, false, 0);
				button->signal_clicked().connect(SigC::slot(*this, &SysMsgDlg::on_sendRequest_button_clicked));		
				sendRequest_vbox->show_all();
			}
			break;
		}
		m_textview.get_buffer()->set_text(msg);
		g_free(msg);
		
		return true;
	}
	return false;
}
