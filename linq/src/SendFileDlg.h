#ifndef __LINQ_SEND_FILE_DLG_H__
#define __LINQ_SEND_FILE_DLG_H__

#include <linq/filesession.h>

#include <gtkmm/window.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/label.h>

#include <string>

class CSendFileDlg : public FileSession, public Gtk::Window
{
public:
	CSendFileDlg(TCPSessionBase *tcp);
	virtual ~CSendFileDlg();

	virtual const char *getPathName(const char *name, uint32 size);
	virtual void onFileStart();
	virtual void onFileProgress(int n);
	virtual void onFileComplete();

protected:
	Gtk::ProgressBar m_progressbar;
	Gtk::Label m_speed_label;

	SigC::Connection update_progress_timeout_conn;
	virtual bool on_update_progress_timeout();

	virtual bool on_delete_event (GdkEventAny  *event);
	virtual void on_stop_clicked ();
private:
	std::string filePath;
	int start_time;
	uint32 transfer_size;
	bool stop_by_myself;

	void createWindow();
};

#endif
