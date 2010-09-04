#include "config.h"
#include "SysMsgListDlg.h"
#include "LinqDB.h"
#include "LinqMsg.h"
#include "intl.h"

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/separator.h>
#include <gtkmm/treestore.h>
#include <gtkmm/scrolledwindow.h>


SysMsgListDlg::SysMsgListDlg()
: LinqWindow(WIN_SYS_MESSAGE_LIST)
{
	signal_delete_event().connect(SigC::slot(*this, &SysMsgListDlg::on_window_delete_event));
	set_resizable(false);
	set_title(_("History event"));
	set_border_width(5);
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);
	
	createTreeView();
	
	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	sw->set_size_request (400, 300);
	sw->add(m_treeview);

	vbox->pack_start(*sw, false, false, 0);

	Gtk::HSeparator *sep;
	sep = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*sep, false, false, 5);
	
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 5));
	vbox->pack_start(*hbox);
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button(_("Close")));
	button->signal_clicked().connect(SigC::slot(*this, &SysMsgListDlg::on_close_button_clicked));
	hbox->pack_end(*button, false, false, 0);
	
	button = Gtk::manage(new Gtk::Button(_("Delete")));
	button->signal_clicked().connect(SigC::slot(*this, &SysMsgListDlg::on_delete_button_clicked));
	hbox->pack_start(*button, false, false, 0);
	button = Gtk::manage(new Gtk::Button(_("Delete All")));
	button->signal_clicked().connect(SigC::slot(*this, &SysMsgListDlg::on_deleteAll_button_clicked));
	hbox->pack_start(*button, false, false, 0);

	show_all();
}

SysMsgListDlg::~SysMsgListDlg()
{
}

void SysMsgListDlg::createTreeView()
{
	Glib::RefPtr<Gtk::TreeStore> treestore;
	treestore = Gtk::TreeStore::create(m_Columns);

	std::list<LinqMsg *> msgList;
	if (LinqDB::loadMsg(NULL, msgList)) {
		Gtk::TreeModel::iterator iter;			
		Gtk::TreeModel::Row row;
		
		std::list<LinqMsg *>::iterator msg_iter;
		for (msg_iter = msgList.begin(); msg_iter != msgList.end(); ++msg_iter) {
			iter = treestore->append();
			row = *iter;
			
			row[m_Columns.from] = (*msg_iter)->from;
			
			delete *msg_iter;
		}
	}
	
	m_treeview.set_rules_hint(true);
	
	m_treeview.set_model(treestore);
	
	m_treeview.append_column(_("From"), m_Columns.from);
	m_treeview.append_column(_("Date"), m_Columns.date);
	m_treeview.append_column(_("Time"), m_Columns.time);
	m_treeview.append_column(_("Content"), m_Columns.content);
	
	m_treeview.signal_button_press_event().connect(SigC::slot(*this, &SysMsgListDlg::on_treeview_button_press));;
}

bool SysMsgListDlg::on_window_delete_event(GdkEventAny * event)
{
	delete this;
	return true;
}

bool SysMsgListDlg::on_treeview_button_press(GdkEventButton * event)
{
	if (event->type==GDK_2BUTTON_PRESS)
	{
		//on_user_list_detail_button_clicked();
		return true;
	}
	else
	{
		return false;
	}
}

void SysMsgListDlg::on_delete_button_clicked()
{
}

void SysMsgListDlg::on_deleteAll_button_clicked()
{
}

void SysMsgListDlg::on_close_button_clicked()
{
	delete this;
}
