#include "config.h"
#include "MainWin_ContactView.h"
#include "LinqMain.h"
#include "SendMsgDlg.h"
#include "MsgManager.h"
#include "intl.h"

#include <gtkmm/button.h>

#include <iostream>

ContactView::ContactView()
: Gtk::VBox(false, 0)
{
	buddy_left_menu = NULL;
	buddy_right_menu = NULL;
	buddy_group_menu = NULL;
	
	show();
	
	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button());
	button->signal_clicked().connect(SigC::slot(*this, &ContactView::on_buddy_button_clicked));
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.status[STATUS_ONLINE]));
	hbox->pack_start(*image, true, true, 0);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Buddy list")));
	hbox->pack_start(*label, true, true, 0);
	button->add(*hbox);
	pack_start(*button, false, false, 0);
	button->show_all();

	m_buddy_treestore = Gtk::TreeStore::create(m_Columns);
	
	Gtk::TreeModel::iterator iter = m_buddy_treestore->append();
	Gtk::TreeModel::Row row = *iter;
	
	row[m_Columns.text] = _("My <b>friends</b>");

	m_buddy_treeview.set_headers_visible(false);
	m_buddy_treeview.set_rules_hint(true);
	
	m_buddy_treeview.set_model(m_buddy_treestore);
	
	Gtk::TreeView::Column* pColumn = Gtk::manage( new Gtk::TreeView::Column("") ); 
	pColumn->pack_start(m_Columns.icon, false); //false = don't expand.
	Gtk::CellRendererText* Renderer = Gtk::manage( new Gtk::CellRendererText() );
	pColumn->pack_start(*Renderer);
	pColumn->add_attribute(Renderer->property_markup(), m_Columns.text);

	m_buddy_treeview.append_column(*pColumn);

	//the next line don't work in gtkmm.
	//m_buddy_treeview.signal_button_press_event().connect(SigC::slot(*this, &ContactView::on_buddy_treeview_button_press));
	m_buddy_treeview.signal_event().connect(SigC::slot(*this, &ContactView::on_buddy_treeview_event));
	m_buddy_treeview.signal_realize().connect(SigC::slot(*this, &ContactView::on_buddy_treeview_realize));

	m_buddy_scrolledwindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	m_buddy_scrolledwindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_buddy_scrolledwindow.add(m_buddy_treeview);

	pack_start(m_buddy_scrolledwindow, true, true, 0);
	m_buddy_scrolledwindow.show_all();

	button = Gtk::manage(new Gtk::Button(_("Group list")));
	button->signal_clicked().connect(SigC::slot(*this, &ContactView::on_group_button_clicked));
	pack_start(*button, false, false, 0);
	button->show_all();


	m_group_treestore = Gtk::TreeStore::create(m_Columns);
	
	iter = m_group_treestore->append();
	row = *iter;
	row[m_Columns.text] = "SCU linux club";

	iter = m_group_treestore->append(row.children());
	Gtk::TreeModel::Row child_row = *iter;
	child_row[m_Columns.icon] = linqMain->skin->linq.face[1];
	child_row[m_Columns.text] = "hu<big>z</big>heng";

	iter = m_group_treestore->append(row.children());
	child_row = *iter;
	child_row[m_Columns.icon] = linqMain->skin->linq.face[2];
	child_row[m_Columns.text] = "<span foreground=\"blue\">zhang</span>yong :)";
	
	iter = m_group_treestore->append();
	row = *iter;
	row[m_Columns.text] = "room120";

	iter = m_group_treestore->append(row.children());
	child_row = *iter;
	child_row[m_Columns.icon] = linqMain->skin->linq.face[3];
	child_row[m_Columns.text] = "aa";

	iter = m_group_treestore->append(row.children());
	child_row = *iter;
	child_row[m_Columns.icon] = linqMain->skin->linq.face[4];
	child_row[m_Columns.text] = "bb";

	m_group_treeview.set_headers_visible(false);
	m_group_treeview.set_rules_hint(true);
	m_group_treeview.show();
	
	m_group_treeview.set_model(m_group_treestore);
	
	pColumn = Gtk::manage( new Gtk::TreeView::Column("name") ); 
	pColumn->pack_start(m_Columns.icon, false); //false = don't expand.
	pColumn->pack_start(m_Columns.text);

	m_group_treeview.append_column(*pColumn);

	m_group_treeview.signal_event().connect(SigC::slot(*this, &ContactView::on_group_treeview_event));
	m_group_treeview.signal_realize().connect(SigC::slot(*this, &ContactView::on_group_treeview_realize));

	m_group_scrolledwindow.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	m_group_scrolledwindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	m_group_scrolledwindow.add(m_group_treeview);

	pack_start(m_group_scrolledwindow, true, true, 0);

	loadContactList();	
}

bool ContactView::buddyTreeModelForeachIterFunc(const Gtk::TreeModel::iterator &iter)
{
	if (m_buddy_treestore->iter_depth(iter) !=0 ) {
		Gtk::TreeModel::Row row = *iter;
		delete row[m_Columns.data];
	}
	return true;
}

ContactView::~ContactView()
{
	m_buddy_treestore->foreach(SigC::slot(*this, &ContactView::buddyTreeModelForeachIterFunc));

	if (buddy_left_menu)
		delete buddy_left_menu;
	if (buddy_right_menu)
		delete buddy_right_menu;
	if (buddy_group_menu)
		delete buddy_group_menu;
}

void ContactView::on_buddy_button_clicked()
{
	m_buddy_scrolledwindow.show();
	m_group_scrolledwindow.hide();
}

void ContactView::on_group_button_clicked()
{
	m_group_scrolledwindow.show();
	m_buddy_scrolledwindow.hide();
}

void ContactView::on_buddy_left_menu_sendmsg_activate()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_buddy_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
  		Gtk::TreeModel::Row row = *iter;
		ContactListNodeData *data = row[m_Columns.data];

		LinqWindow *win;
		win = linqMain->findWindow(WIN_SEND_MESSAGE);		
		if (win) {
			SendMsgDlg *dlg;
			dlg = (SendMsgDlg *)win;
			dlg->onSendMessage(data->name.c_str());
		}
		else {
			new SendMsgDlg(data->name.c_str());
		}	
	}
}

void ContactView::on_buddy_left_menu_sendfile_activate()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_buddy_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		Gtk::TreeModel::Row row = *iter;
		ContactListNodeData *data = row[m_Columns.data];
		linqMain->sendTCPRequest("SENDFILE", data->name.c_str(), "nick", "reason");
	}	
}

void ContactView::on_buddy_left_menu_sendmail_activate()
{
}

void ContactView::on_buddy_left_menu_chatlog_activate()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_buddy_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		Gtk::TreeModel::Row row = *iter;
		ContactListNodeData *data = row[m_Columns.data];
		
		LinqWindow *win;
		win = linqMain->findWindow(WIN_MSG_MANAGER);
		if (win) {
			MsgManager *dlg;
			dlg = (MsgManager *)win;
			dlg->doSelect(data->name.c_str());
			dlg->present();
		}
		else {
			new MsgManager(data->name.c_str());
		}
	}
}

void ContactView::on_buddy_left_menu_viewinfo_activate()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_buddy_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		Gtk::TreeModel::Row row = *iter;
		ContactListNodeData *data = row[m_Columns.data];
		linqMain->main_win->getContactInfo(data->name.c_str());
	}
}

void ContactView::on_buddy_right_menu_addGroup_activate()
{
}

void ContactView::on_buddy_right_menu_onlyShowOnlineUser_activate()
{
}

void ContactView::on_buddy_right_menu_delete_activate()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_buddy_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		Gtk::TreeModel::Row row = *iter;
		ContactListNodeData *data = row[m_Columns.data];
		linqMain->getUDPSession()->delContact(data->name.c_str());
		delete data;
		m_buddy_treestore->erase(iter);
	}
}

void ContactView::on_buddy_right_menu_rename_activate()
{
}

void ContactView::on_buddy_treeview_realize()
{
	m_buddy_treeview.expand_all();
	std::cout << "realize" << std::endl;
}

bool ContactView::on_buddy_treeview_event(GdkEvent *e)
{
	if (e->type != GDK_BUTTON_PRESS)
		return false;
	GdkEventButton *event = (GdkEventButton *)e;
	
	if (event->window != m_buddy_treeview.get_bin_window()->gobj())
		return false;
	if ((event->button != 1) && (event->button !=3))
		return false;
	
	Gtk::TreeModel::Path path;
	Gtk::TreeViewColumn *column;
	int cell_x, cell_y;
	if (!m_buddy_treeview.get_path_at_pos((int)(event->x), (int)(event->y), path, column, cell_x, cell_y)) {
		//show menu...
		return false;
	}
	bool return_val=false;
	
	Gtk::TreeModel::iterator iter = m_buddy_treestore->get_iter(path);
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_buddy_treeview.get_selection();

	if (event->button == 1) {		
		if (m_buddy_treestore->iter_depth(iter) == 0) {
			return_val = false;
		}
		else {
			if (!buddy_left_menu) {
				buddy_left_menu = Gtk::manage(new Gtk::Menu());
				Gtk::Menu::MenuList& menulist = buddy_left_menu->items();
				Gtk::Image *image;
				image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.buddy_menu.send_message));
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Send message"), *image, SigC::slot(*this, &ContactView::on_buddy_left_menu_sendmsg_activate)));
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("Send _file"), SigC::slot(*this, &ContactView::on_buddy_left_menu_sendfile_activate)));
				menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
				image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.buddy_menu.send_mail));
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("Send _mail"), *image, SigC::slot(*this, &ContactView::on_buddy_left_menu_sendmail_activate)));
				menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
				image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.buddy_menu.chat_log));
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Chat log"), *image, SigC::slot(*this, &ContactView::on_buddy_left_menu_chatlog_activate)));
				menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
				image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.buddy_menu.view_info));
				menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_View info"), *image, SigC::slot(*this, &ContactView::on_buddy_left_menu_viewinfo_activate)));

				buddy_left_menu->show_all();
			}
			buddy_left_menu->popup(event->button, event->time);
			
			refTreeSelection->select(path);
			
			return_val = true;
		}				
	}
	else if (event->button == 3) {
		if (m_buddy_treestore->iter_depth(iter) == 0) {
			if (!buddy_group_menu) {
				buddy_group_menu = Gtk::manage(new Gtk::Menu());
				
				Gtk::Menu::MenuList& menulist = buddy_group_menu->items();
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Add buddy")));
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Delete group")));
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Rename")));
				
				buddy_group_menu->show_all();				
			}
			buddy_group_menu->popup(event->button, event->time);			
			refTreeSelection->select(path);			
			return_val = true;
		}
		else {
			if (!buddy_right_menu) {
				buddy_right_menu = Gtk::manage(new Gtk::Menu());
				
				Gtk::Menu::MenuList& menulist = buddy_right_menu->items();
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Add group"), SigC::slot(*this, &ContactView::on_buddy_right_menu_addGroup_activate)));
				menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Only show online user"), SigC::slot(*this, &ContactView::on_buddy_right_menu_onlyShowOnlineUser_activate)));
				menulist.push_back(Gtk::Menu_Helpers::SeparatorElem());
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Delete from this group"), SigC::slot(*this, &ContactView::on_buddy_right_menu_delete_activate)));
				menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Rename"), SigC::slot(*this, &ContactView::on_buddy_right_menu_rename_activate)));
			
				buddy_right_menu->show_all();
			}
			buddy_right_menu->popup(event->button, event->time);			
			refTreeSelection->select(path);			
			return_val = true;
		}				
	}	
	return return_val;
}

void ContactView::on_group_treeview_realize()
{
	m_group_treeview.expand_all();
}

bool ContactView::on_group_treeview_event(GdkEvent *e)
{
	if (e->type != GDK_BUTTON_PRESS)
		return false;
	//GdkEventButton *event = (GdkEventButton *)e;

	return false;
}

bool ContactView::getContactIter(const char *name, Gtk::TreeModel::iterator &iter)
{
	Gtk::TreeModel::Children children = m_buddy_treestore->children();
	for(Gtk::TreeModel::Children::iterator child_iter = children.begin(); child_iter != children.end(); ++child_iter) {
		Gtk::TreeModel::Row row = *child_iter;
		if (getContactIter(name, iter, row.children()))
			return true;
	}
	return false;
}

bool ContactView::getContactIter(const char *name, Gtk::TreeModel::iterator &iter, const Gtk::TreeModel::Children &children)
{
	Gtk::TreeModel::Children::iterator child_iter;
	for (child_iter = children.begin(); child_iter!=children.end(); ++child_iter) {
		Gtk::TreeModel::Row row = *child_iter;
		ContactListNodeData *data;
		data = row[m_Columns.data];
		if (data->name == name) {
			iter = child_iter;
			return true;
		}
		//if (getContactIter(name, iter, row.children()))
			//return true;
	}
	return false;
}

void ContactView::loadContactList()
{
	std::list<std::string *> contactList;
	if (LinqDB::loadContactList(contactList)) {
		Gtk::TreeModel::iterator iter;
		Gtk::TreeModel::Children children = m_buddy_treestore->children();
		iter = children.begin();
		std::list<std::string *>::iterator contact_iter;
		for (contact_iter=contactList.begin(); contact_iter!=contactList.end(); ++contact_iter) {
			Gtk::TreeModel::Row row = *iter;

			ContactListNodeData *data;
			data = new ContactListNodeData;
			data->name = **contact_iter;
			data->status = STATUS_OFFLINE;
	
			Gtk::TreeModel::iterator child_iter;
			child_iter = m_buddy_treestore->append(row.children());
			row = *child_iter;
			row[m_Columns.icon] = linqMain->skin->linq.status[STATUS_OFFLINE];
			row[m_Columns.text] = **contact_iter;
			row[m_Columns.data] = data;	

			delete *contact_iter;			
		}
	}
}

void ContactView::onUserOnline(const char *name)
{
	g_print("onUserOnline: %s\n", name);
	Gtk::TreeModel::iterator iter;
	if (getContactIter(name, iter)) {
		Gtk::TreeModel::Row row = *iter;
		ContactListNodeData *data;
		data = row[m_Columns.data];		
		if (data->status != STATUS_ONLINE) {
			data->status = STATUS_ONLINE;
			row[m_Columns.icon] = linqMain->skin->linq.face[1];
		}
	}
	else {
		Gtk::TreeModel::Children children = m_buddy_treestore->children();
		iter = children.begin();
		Gtk::TreeModel::Row row = *iter;

		ContactListNodeData *data;
		data = new ContactListNodeData;
		data->name = name;
		data->status = STATUS_ONLINE;
	
		Gtk::TreeModel::iterator child_iter;
		child_iter = m_buddy_treestore->append(row.children());
		row = *child_iter;
		row[m_Columns.icon] = linqMain->skin->linq.face[1];
		row[m_Columns.text] = name;
		row[m_Columns.data] = data;
	}
}

void ContactView::onUserOffline(const char *name)
{
	g_print("onUserOffline: %s\n", name);
	Gtk::TreeModel::iterator iter;
	if (getContactIter(name, iter)) {
		Gtk::TreeModel::Row row = *iter;
		ContactListNodeData *data;
		data = row[m_Columns.data];		
		if (data->status != STATUS_OFFLINE) {
			data->status = STATUS_OFFLINE;
			row[m_Columns.icon] = linqMain->skin->linq.status[STATUS_OFFLINE];
		}
		
		/*Gtk::TreeModel::Row row = *iter;
		delete row[m_Columns.data];
		m_buddy_treestore->erase(iter);*/
	}	
}

void ContactView::onUserStatus(const char *name, uint32 status)
{
	
}

void ContactView::onContactInfoReply(CONTACT_INFO &c)
{
}
