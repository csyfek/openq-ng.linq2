#include "config.h"
#include "SendFileDlg.h"
#include "LinqMain.h"
#include "intl.h"

#include <gtkmm/fileselection.h>
#include <gtkmm/messagedialog.h>

CSendFileDlg::CSendFileDlg(TCPSessionBase *tcp) : FileSession(tcp)
{
	transfer_size = 0;
	stop_by_myself = false;
	
	createWindow();	
	if (tcpSession->isSender()) {
		show();
		Gtk::FileSelection dialog(_("Choose the file which you want to send"));
		dialog.set_transient_for(*this);
		int result = dialog.run();
		if (result == Gtk::RESPONSE_OK) {
			filePath = dialog.get_filename();
			sendFileInfo(filePath.c_str());
		}
		else
			tcpSession->destroy();
	}
}

CSendFileDlg::~CSendFileDlg()
{
}

const char *CSendFileDlg::getPathName(const char *name, uint32 size)
{	
	show();
	gchar *title = g_strdup_printf(_("Receive file. size: %u K. Save as..."), size/1024);
	Gtk::FileSelection dialog(title);	
	g_free(title);
	dialog.set_transient_for(*this);
	dialog.set_filename(name);
	
	int result = dialog.run();
	if (result == Gtk::RESPONSE_OK) {
		filePath = dialog.get_filename();		
		return filePath.c_str();
	}
	else {
		tcpSession->destroy();
		return NULL;
	}
}

void CSendFileDlg::onFileProgress(int n)
{
	transfer_size += n;
}

void CSendFileDlg::onFileComplete()
{
	if (update_progress_timeout_conn.connected())
		update_progress_timeout_conn.disconnect();
	
	if (stop_by_myself)
		return;
	
	int costtime = (int)(difftime(time(NULL), start_time));
	if (costtime==0) //can't be div.
		costtime = 1;
	int speed = (int)((transfer_size / costtime)/1024);
	Glib::ustring filename = Glib::filename_to_utf8(filePath);
	if (transfer_size == fileSize) {
		if (tcpSession->isSender()) {
			gchar *text;
			text = g_strdup_printf(_("File send successful!\nsend %u bytes cost %d seconds, %d k/s\nfilename: %s"), transfer_size, costtime, speed,filename.c_str());
			Gtk::MessageDialog dialog(*(linqMain->main_win), text, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
			g_free(text);
			dialog.show();
		}
		else {
			gchar *text;
			text = g_strdup_printf(_("File receive successful!\nreceive %u bytes cost %d seconds, %d k/s\nsave as %s"), transfer_size, costtime,speed,filename.c_str());
			Gtk::MessageDialog dialog(*(linqMain->main_win), text, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
			g_free(text);
			dialog.show();
		}
	}
	else {
		if (tcpSession->isSender()) {
			gchar *text;
			text = g_strdup_printf(_("Error occupy while sending file!\nsend %u bytes,but it should be %u bytes.\ncost %d seconds, %d k/s\nfilename: %s"), transfer_size, fileSize, costtime,speed,filename.c_str());
			Gtk::MessageDialog dialog(*(linqMain->main_win), text, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
			g_free(text);
			dialog.show();
		}
		else {
			gchar *text;
			text = g_strdup_printf(_("Error occupy when receiving file!\nreceive %u bytes,but it should be %u bytes.\ncost %d seconds, %d k/s\nsave as %s"), transfer_size, fileSize,costtime,speed,filename.c_str());
			Gtk::MessageDialog dialog(*(linqMain->main_win), text, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
			g_free(text);
			dialog.show();
		}
	}
}

bool CSendFileDlg::on_update_progress_timeout()
{
	int nowtime = (int)(difftime(time(NULL), start_time));
	if (fileSize==0 || nowtime==0) // it can't be div.
		return true;
	m_progressbar.set_fraction(((gfloat)(transfer_size) / (gfloat)(fileSize)));
	
	gchar *text = g_strdup_printf(_("%8d k/s"),(gint) ((transfer_size / nowtime)/1024));
	m_speed_label.set_text(text);
	g_free(text);
	return true;
}

bool CSendFileDlg::on_delete_event (GdkEventAny  *event)
{
	stop_by_myself = true;
	onClose();
	return true;
}

void CSendFileDlg::on_stop_clicked ()
{
	stop_by_myself = true;
	onClose();
}

void CSendFileDlg::onFileStart()
{	
	start_time = time(NULL);	
}

void CSendFileDlg::createWindow()
{
	signal_delete_event().connect(SigC::slot(*this, &CSendFileDlg::on_delete_event));
	set_resizable(false);
	
	if (tcpSession->isSender())
		set_title(_("Sending file"));
	else
		set_title (_("Receiving file"));
	set_border_width(8);

	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);

	Glib::ustring filename;
	filename = Glib::filename_to_utf8(filePath);
	
	gchar *text;
	if (tcpSession->isSender())
		text = g_strdup_printf(_("Sending file. %s size: %u bytes"), filename.c_str(), fileSize);		
	else
		text = g_strdup_printf(_("Receiving file. %s size: %u bytes"), filename.c_str(), fileSize);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(text));
	g_free(text);
	vbox->pack_start(*label);
	
	vbox->pack_start(m_progressbar);

	m_speed_label.set_text(_("Waiting..."));
	vbox->pack_start(m_speed_label);

	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button(_("stop")));	
	vbox->pack_start(*button);
	button->signal_clicked().connect(SigC::slot(*this, &CSendFileDlg::on_stop_clicked));
	
	vbox->show_all();

	update_progress_timeout_conn = Glib::signal_timeout().connect(SigC::slot(*this, &CSendFileDlg::on_update_progress_timeout), 500);	
}
