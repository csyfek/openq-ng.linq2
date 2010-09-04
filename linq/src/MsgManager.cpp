#include "config.h"
#include "MsgManager.h"
#include "LinqMain.h"
#include "LinqMsg.h"
#include "skin.h"
#include "intl.h"

#include <gtkmm/box.h>
#include <gtkmm/notebook.h>
#include <gtkmm/paned.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>
#include <gtkmm/toolbar.h>
#include <gtkmm/scrolledwindow.h>
#include <gtkmm/treestore.h>
#include <gtkmm/stock.h>
#include <gtkmm/image.h>


MsgManager::MsgManager(const char *name)
: LinqWindow(WIN_MSG_MANAGER)
{
	signal_delete_event().connect(SigC::slot(*this, &MsgManager::on_window_delete_event));
	set_title(_("Message manager"));
	set_border_width(5);
	
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);
	
	vbox->pack_start(createMenu(), false, false, 0);
	vbox->pack_start(createToolbar(), false, false, 0);
	
	Gtk::HPaned *hpaned;
	hpaned = Gtk::manage(new Gtk::HPaned());
	vbox->pack_start(*hpaned, true, true, 0);
	hpaned->add1(createLeftTree());
	
	Gtk::VPaned *vpaned;
	vpaned = Gtk::manage(new Gtk::VPaned());
	hpaned->add2(*vpaned);
	
	Gtk::Notebook *notebook;
	notebook = Gtk::manage(new Gtk::Notebook());
	vpaned->add1(*notebook);	
	notebook->set_tab_pos(Gtk::POS_BOTTOM);
	page_msg = createMsgList();
	page_msg_label = Gtk::manage(new Gtk::Label(_("Chat Log")));
	notebook->append_page(*page_msg, *page_msg_label);
	page_info = createInfoList();
	notebook->append_page(*page_info, _("Info"));
	page_remark = createRemarkList();
	notebook->append_page(*page_remark, _("Remark"));
	
	vpaned->add2(createMsgViewer());
	
	nowSeletedItemType = SELECT_ITEM_TYPE_BUDDY;
	
	if (name)
		doSelect(name);
	
	show_all();
}

MsgManager::~MsgManager()
{
}

Gtk::Widget &MsgManager::createMenu()
{
	Gtk::MenuBar *menubar;
	menubar = Gtk::manage(new Gtk::MenuBar());

	Gtk::Menu *menu_file;
	menu_file = Gtk::manage(new Gtk::Menu());
	Gtk::Menu::MenuList& menulist = menu_file->items();
	menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::SAVE_AS, SigC::slot(*this, &MsgManager::on_menu_file_saveAs)));	
	menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::CLOSE, SigC::slot(*this, &MsgManager::on_menu_file_close)));	
	menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_File"), *menu_file));

	Gtk::Menu *menu_edit;
	menu_edit = Gtk::manage(new Gtk::Menu());
	menulist = menu_edit->items();
	menulist.push_back(Gtk::Menu_Helpers::StockMenuElem(Gtk::Stock::DELETE, SigC::slot(*this, &MsgManager::on_menu_edit_delete)));	
	menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_Edit"), *menu_edit));
	
	Gtk::Menu *menu_view;
	menu_view = Gtk::manage(new Gtk::Menu());
	menulist = menu_view->items();
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GOTO_FIRST, Gtk::ICON_SIZE_MENU));
	menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_First page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_firstPage)));	
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GO_BACK, Gtk::ICON_SIZE_MENU));
	menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Previous page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_previousPage)));	
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GO_FORWARD, Gtk::ICON_SIZE_MENU));
	menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Next page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_nextPage)));	
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GOTO_LAST, Gtk::ICON_SIZE_MENU));
	menulist.push_back(Gtk::Menu_Helpers::ImageMenuElem(_("_Last page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_lastPage)));	
	menubar->items().push_back(Gtk::Menu_Helpers::MenuElem(_("_View"), *menu_view));
	return *menubar;
}

Gtk::Widget &MsgManager::createToolbar()
{
	Gtk::Toolbar *toolbar;
	toolbar = Gtk::manage(new Gtk::Toolbar());
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SAVE_AS, Gtk::ICON_SIZE_SMALL_TOOLBAR));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem(_("Save as"), *image, SigC::slot(*this, &MsgManager::on_menu_file_saveAs), _("Save the messages to a txt file")));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::Space());
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::DELETE, Gtk::ICON_SIZE_SMALL_TOOLBAR));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem(_("Delete"), *image, SigC::slot(*this, &MsgManager::on_menu_edit_delete), _("delete this msg")));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::Space());
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GOTO_FIRST, Gtk::ICON_SIZE_SMALL_TOOLBAR));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem(_("First page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_firstPage), _("view the first page")));
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GO_BACK, Gtk::ICON_SIZE_SMALL_TOOLBAR));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem(_("Previous page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_previousPage), _("view the first page")));
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GO_FORWARD, Gtk::ICON_SIZE_SMALL_TOOLBAR));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem(_("Next page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_nextPage), _("view the first page")));
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::GOTO_LAST, Gtk::ICON_SIZE_SMALL_TOOLBAR));
	toolbar->tools().push_back(Gtk::Toolbar_Helpers::ButtonElem(_("Last page"), *image, SigC::slot(*this, &MsgManager::on_menu_view_lastPage), _("view the first page")));
	return *toolbar;
}

Gtk::Widget &MsgManager::createLeftTree()
{
	Glib::RefPtr<Gtk::TreeStore> treestore;
	treestore = Gtk::TreeStore::create(m_RosterColumns);
	
	m_RosterTreeview.set_headers_visible(false);
	m_RosterTreeview.set_rules_hint(true);
	
	Gtk::TreeView::Column* pColumn = Gtk::manage( new Gtk::TreeView::Column("") ); 
	pColumn->pack_start(m_RosterColumns.icon, false); //false = don't expand.
	Gtk::CellRendererText* Renderer = Gtk::manage( new Gtk::CellRendererText() );
	pColumn->pack_start(*Renderer);
	pColumn->add_attribute(Renderer->property_markup(), m_RosterColumns.text);
	m_RosterTreeview.append_column(*pColumn);


	Gtk::TreeModel::iterator iter = treestore->append();
	Gtk::TreeModel::Row row = *iter;		
	row[m_RosterColumns.icon] = linqMain->skin->linq.face[1];
	row[m_RosterColumns.text] = linqMain->myInfo.name;
	row[m_RosterColumns.name] = "#root";
	iter = treestore->append(row.children());
	Gtk::TreeModel::Row child_row = *iter;
	child_row[m_RosterColumns.icon] = linqMain->skin->linq.buddy;
	child_row[m_RosterColumns.text] = _("My Friends");
	child_row[m_RosterColumns.name] = "#myfriends";

	std::list<std::string *> contactList;
	if (LinqDB::loadContactList(contactList)) {
		std::list<std::string *>::iterator contact_iter;
		Gtk::TreeModel::iterator friend_child_iter;
		Gtk::TreeModel::Row friend_child_row;
		for (contact_iter=contactList.begin(); contact_iter!=contactList.end(); ++contact_iter) {			
			friend_child_iter = treestore->append(child_row.children());
			friend_child_row = *friend_child_iter;
			friend_child_row[m_RosterColumns.icon] = linqMain->skin->linq.face[5];;
			friend_child_row[m_RosterColumns.text] = **contact_iter;
			friend_child_row[m_RosterColumns.name] = **contact_iter;	

			delete *contact_iter;			
		}
	}

	iter = treestore->append(row.children());
	child_row = *iter;
	child_row[m_RosterColumns.icon] = linqMain->skin->linq.sysmsg;
	child_row[m_RosterColumns.text] = _("System Message");
	child_row[m_RosterColumns.name] = "#sysmsg";

	
	m_RosterTreeview.set_model(treestore);
	m_RosterTreeview.get_selection()->signal_changed().connect(SigC::slot(*this, &MsgManager::on_RosterTreeview_selection_changed));

	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	sw->add(m_RosterTreeview);
	return *sw;
}

Gtk::Widget *MsgManager::createMsgList()
{
	m_MsgListstore = Gtk::ListStore::create(m_MsgColumns);
	
	m_MsgTreeview.set_rules_hint(true);
	m_MsgTreeview.append_column(_("Sender"), m_MsgColumns.sender);
	m_MsgTreeview.append_column(_("Date"), m_MsgColumns.date);
	m_MsgTreeview.append_column(_("Time"), m_MsgColumns.time);
	m_MsgTreeview.append_column(_("Content"), m_MsgColumns.content);
	
	m_MsgTreeview.set_model(m_MsgListstore);
	m_MsgTreeview.get_selection()->signal_changed().connect(SigC::slot(*this, &MsgManager::on_MsgTreeview_selection_changed));
	
	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	sw->add(m_MsgTreeview);

	return sw;
}

Gtk::Widget *MsgManager::createInfoList()
{
	m_InfoListstore = Gtk::ListStore::create(m_InfoColumns);
	
	m_InfoTreeview.set_rules_hint(true);
	m_InfoTreeview.append_column(_("Item"), m_InfoColumns.item);
	m_InfoTreeview.append_column(_("Content"), m_InfoColumns.content);
	
	m_InfoTreeview.set_model(m_InfoListstore);

	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	sw->add(m_InfoTreeview);
	return sw;
}

Gtk::Widget *MsgManager::createRemarkList()
{
	m_RemarkListstore = Gtk::ListStore::create(m_InfoColumns);
	
	m_RemarkTreeview.set_rules_hint(true);
	m_RemarkTreeview.append_column(_("Item"), m_InfoColumns.item);
	m_RemarkTreeview.append_column(_("Content"), m_InfoColumns.content);
	
	m_RemarkTreeview.set_model(m_RemarkListstore);

	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	
	sw->add(m_RemarkTreeview);
	return sw;
}

Gtk::Widget &MsgManager::createMsgViewer()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 0));
	vbox ->pack_start(*hbox, false, false, 0);
	m_MsgViewer_sender_icon.set(linqMain->skin->linq.face[0]);
	hbox->pack_start(m_MsgViewer_sender_icon, false, false, 0);
	hbox->pack_start(m_MsgViewer_sender_label, false, false, 0);
	
	vbox->pack_start(m_MsgViewer_textview, false, false, 0);	
	return *vbox;
}

void MsgManager::doSelect(const char *name)
{
}


bool MsgManager::on_window_delete_event(GdkEventAny * event)
{
	delete this;
	return true;
}

void MsgManager::on_menu_file_saveAs()
{
}

void MsgManager::on_menu_file_close()
{
	delete this;
}

void MsgManager::on_menu_edit_delete()
{
}

void MsgManager::on_menu_view_firstPage()
{
}

void MsgManager::on_menu_view_previousPage()
{
}

void MsgManager::on_menu_view_nextPage()
{
}

void MsgManager::on_menu_view_lastPage()
{
}

void MsgManager::on_RosterTreeview_selection_changed()
{
	Gtk::TreeModel::iterator iter = m_RosterTreeview.get_selection()->get_selected();
	if (iter) {
  		Gtk::TreeModel::Row row = *iter;
		Glib::ustring name = row[m_RosterColumns.name];
		if (name == "#sysmsg") {
			if (nowSeletedItemType != SELECT_ITEM_TYPE_SYSMSG) {
				nowSeletedItemType = SELECT_ITEM_TYPE_SYSMSG;
				page_msg_label->set_label(_("System Message"));
				page_info->hide();
				page_remark->hide();				
			}
			
			m_MsgListstore->clear();
			std::list<LinqMsg *> msgList;
			if (LinqDB::loadMsg(NULL, msgList)) {
				Gtk::TreeModel::iterator msg_treeiter;
				Gtk::TreeModel::Row msg_row;
		
				std::list<LinqMsg *>::iterator msg_iter;
				for (msg_iter = msgList.begin(); msg_iter != msgList.end(); ++msg_iter) {
					msg_treeiter = m_MsgListstore->append();
					msg_row = *msg_treeiter;
			
					msg_row[m_MsgColumns.sender] = (*msg_iter)->from;
			
					delete *msg_iter;
				}
			}
		}
		else {
			if (nowSeletedItemType != SELECT_ITEM_TYPE_BUDDY) {
				nowSeletedItemType = SELECT_ITEM_TYPE_BUDDY;
				page_msg_label->set_label(_("Chat Log"));
				page_info->show();
				page_remark->show();
			}

			m_MsgListstore->clear();
			std::list<LinqMsg *> msgList;
			if (LinqDB::loadMsg(name.c_str(), msgList)) {
				Gtk::TreeModel::iterator msg_treeiter;
				Gtk::TreeModel::Row msg_row;
		
				std::list<LinqMsg *>::iterator msg_iter;
				for (msg_iter = msgList.begin(); msg_iter != msgList.end(); ++msg_iter) {
					msg_treeiter = m_MsgListstore->append();
					msg_row = *msg_treeiter;
			
					msg_row[m_MsgColumns.sender] = (*msg_iter)->from;
			
					delete *msg_iter;
				}
			}
		}
	}
}

void MsgManager::on_MsgTreeview_selection_changed()
{
	Gtk::TreeModel::iterator iter = m_MsgTreeview.get_selection()->get_selected();
	if (iter) {
  		//Gtk::TreeModel::Row row = *iter;
	}
}
