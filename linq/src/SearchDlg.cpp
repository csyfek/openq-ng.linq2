#include "config.h"
#include "SearchDlg.h"
#include "LinqMain.h"
#include "intl.h"

#include <stdio.h>

#include <gtkmm/treemodel.h>
#include <gtkmm/separator.h>
#include <gtkmm/frame.h>
#include <gtkmm/messagedialog.h>

LinqSearchResult::LinqSearchResult(SEARCH_RESULT &result)
{
	name = result.name;
	status = result.status;
	nick = result.nick;
	auth = result.auth;
	gender = result.gender;
	age = result.age;
}

SearchDlg::SearchDlg()
: LinqWindow(WIN_SEARCH),
  m_back_button(_("< Back")),
  m_next_button(_("Next >")),
  m_cancel_button(_("Cancel")),
  m_radio_online_user(_("Random search online user")),
  m_radio_custom_search(_("Custom search")),
  m_radio_remote_friend(_("Remote friends")),
  m_radio_group_list(_("Show group list")),
  m_radio_group_num(_("Group number")),
  m_radio_create_group(_("Create group")),
  m_add_friend_hbox(false, 8),
  m_user_list_previous_button(_("Previous page")),
  m_user_list_next_button(_("Next page")),
  m_user_list_detail_button(_("Detail")),
  m_user_list_all_button(_("All pages"))
{
	signal_delete_event().connect(SigC::slot(*this, &SearchDlg::on_window_delete_event));
	set_resizable(false);
	set_title(_("Search"));
	set_border_width(8);

	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);

	create_notebook();
	vbox->pack_start(m_notebook, false, false, 10);

	Gtk::HSeparator *separator;
	separator = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*separator, false, true, 10);

	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 10));
	vbox->pack_start(*hbox);
	m_back_button.set_sensitive(false);
	hbox->pack_start(m_back_button);
	m_back_button.signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_back_clicked));
	hbox->pack_start(m_next_button);
	m_next_button.signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_next_clicked));
	hbox->pack_start(m_cancel_button);
	m_cancel_button.signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_cancel_clicked));
	
	show_all();
}

SearchDlg::~SearchDlg()
{
	std::list< std::list< LinqSearchResult > * >::iterator iter;
	for (iter = searchUserResultPageList.begin(); iter != searchUserResultPageList.end(); ++iter) {
		delete (*iter);
	}
}

bool SearchDlg::on_window_delete_event (GdkEventAny *event)
{
	delete this;
	return true;
}

void SearchDlg::on_back_clicked ()
{
	SearchPage back_page;
	switch (nb_current_page)
	{
		case NB_CUSTOM_SEARCH:
			back_page = NB_CHOOSE;
			break;
		case NB_USER_LIST:
			if (nb_choose_radio == NB_CHOOSE_RADIO_RANDOM_SEARCH)
				back_page = NB_CHOOSE;
			else if (nb_choose_radio == NB_CHOOSE_RADIO_CUSTOM_SEARCH)
				back_page = NB_CUSTOM_SEARCH;
			else if (nb_choose_radio == NB_CHOOSE_RADIO_REMOTE_FRIEND)
				back_page = NB_SERVER_LIST;
			else
				back_page = NB_CHOOSE;
			break;
		case NB_ADD_FRIEND:
			back_page = NB_USER_LIST;
			break;
		case NB_GROUP_TYPE_LIST:
			back_page = NB_CHOOSE;
			break;
		case NB_GROUP_LIST:
			if (nb_choose_radio == NB_CHOOSE_RADIO_GROUP_LIST)
				back_page = NB_GROUP_TYPE_LIST;
			else if (nb_choose_radio == NB_CHOOSE_RADIO_GROUP_NUM)
				back_page = NB_GROUP_NUM;
			else
				back_page = NB_CHOOSE;
			break;
		case NB_CREATE_GROUP:
			back_page = NB_GROUP_TYPE_LIST;
			break;
		case NB_CREATE_GROUP_RESULT:
			if (nb_choose_radio == NB_CHOOSE_RADIO_GROUP_LIST)
				back_page = NB_GROUP_LIST;
			else if (nb_choose_radio == NB_CHOOSE_RADIO_GROUP_NUM)
				back_page = NB_GROUP_LIST;
			else if (nb_choose_radio == NB_CHOOSE_RADIO_CREATE_GROUP)
				back_page = NB_CREATE_GROUP;
			else
				back_page = NB_CHOOSE;
			break;
		case NB_GROUP_NUM:
			back_page = NB_CHOOSE;
			break;
		default:
			back_page = NB_CHOOSE;
	}
	m_notebook.set_current_page(back_page);
	nb_current_page = back_page;	
	if (nb_current_page==NB_CHOOSE)
		m_back_button.set_sensitive(false);
}

void SearchDlg::clean_search_user_result ()
{
	std::list< std::list< LinqSearchResult > * >::iterator iter;
	for (iter = searchUserResultPageList.begin(); iter != searchUserResultPageList.end(); ++iter) {
		delete (*iter);
	}
	searchUserResultPageList.clear();

	m_user_list_liststore->clear();
	
	m_user_list_info_label.set_text(_("Searching..."));
	m_user_list_previous_button.set_sensitive(false);
	m_user_list_next_button.set_sensitive(false);
	m_user_list_all_button.set_sensitive(false);
	
	search_user_result_current_page = -2; //so it is not equal to -1 and 0,1,2,3....so can do a fresh.
}

/*
static void
search_dialog_clean_search_group_result (struct search_dialog *sd)
{
	struct search_group_info *group;
	while (sd->search_group_list)
	{
		group = (struct search_group_info *)(sd->search_group_list->data);
		g_free(group->name);
		g_free(group);
		sd->search_group_list = g_slist_remove(sd->search_group_list,group);
	}

	gtk_list_store_clear(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sd->group_list_treeview))));
	gtk_label_set_text(GTK_LABEL(sd->group_list_info_label),_("Searching..."));
}

static void
search_dialog_clean_search_server_result (struct search_dialog *sd)
{
	struct search_server_info *server;
	while (sd->search_server_list)
	{
		server = (struct search_server_info *)(sd->search_server_list->data);
		g_free(server->domain);
		g_free(server->desc);
		g_free(server);
		sd->search_server_list = g_slist_remove(sd->search_server_list,server);
	}
}
*/

void SearchDlg::refresh_search_user_result (gint refresh_page)
{
	if (refresh_page==search_user_result_current_page)
		return;
		
	std::list< std::list< LinqSearchResult > * >::iterator page_iter;

	gint total_page;
	total_page= searchUserResultPageList.size();
	page_iter = searchUserResultPageList.begin();
	if (refresh_page!=-1) //all page.
		for (int i=0; i<refresh_page;i++)
			++page_iter;

	m_user_list_liststore->clear();

	Gtk::TreeModel::iterator iter;
	Gtk::TreeModel::Row row;
	std::list< LinqSearchResult >::iterator result_iter;
	while (page_iter != searchUserResultPageList.end())
	{
		result_iter = (*(*page_iter)).begin();
		while (result_iter != (*(*page_iter)).end()) {
			LinqSearchResult *result;
			result = &(*result_iter);
			iter = m_user_list_liststore->append();
			row = *iter;
			row[m_user_list_columns.name] = result->name;
			row[m_user_list_columns.nick] = result->nick;
			row[m_user_list_columns.status] = result->status;
			row[m_user_list_columns.auth] = result->auth;
			row[m_user_list_columns.gender] = result->gender;
			row[m_user_list_columns.age] = result->age;
			++result_iter;
		}
		if (refresh_page==-1)
			++page_iter;
		else
			page_iter = searchUserResultPageList.end();
	}
	if (refresh_page==total_page-1) //the last page
	{
/*		page_list = g_slist_last(sd->search_user_result_page_list);
		user_info_list = (GSList *)(page_list->data);
		if (!user_info_list) //find no user.
		{
			sd->customSearch_startUIN = -1;
		}
		else
		{
			user_info_list = g_slist_last(user_info_list);
			user = (struct search_user_info*)(user_info_list->data);			
			sd->customSearch_startUIN = user->uin; //the last user has the max number.
		}*/
	}

	gchar text[256];
	if (refresh_page != -1)
		sprintf(text,_("Result of page %d(%d)"),refresh_page+1,total_page);
	else
		sprintf(text,_("All result(%d page in total)"),total_page);
	m_user_list_info_label.set_text(text);
	if (refresh_page!=0)
		m_user_list_previous_button.set_sensitive(true);
	else
		m_user_list_previous_button.set_sensitive(false);
	m_user_list_next_button.set_sensitive(true);
	m_user_list_all_button.set_sensitive(true);
	
	search_user_result_current_page = refresh_page;
	//gtk_window_present(GTK_WINDOW(window));
}

/*
void
search_dialog_refresh_search_group_result (struct search_dialog *sd)
{
	GSList *group_info_list;
	struct search_group_info *group;
	GtkListStore *list_store;
	GtkTreeIter iter;
	
	list_store=GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sd->group_list_treeview)));
	gtk_list_store_clear(list_store);
	group_info_list = sd->search_group_list;
	while (group_info_list)
	{
		group = (struct search_group_info*)(group_info_list->data);
		gtk_list_store_append (list_store, &iter);
		gtk_list_store_set (list_store, &iter,
			  	0, group->id,
				1,group->name,
				2,group->num,
				-1);
		group_info_list = g_slist_next(group_info_list);
	}
	gtk_label_set_text(GTK_LABEL(sd->group_list_info_label),_("Completed"));
}

void
search_dialog_refresh_search_server_result (struct search_dialog *sd)
{
	GSList *server_info_list;
	struct search_server_info *server;
	GtkListStore *list_store;
	GtkTreeIter iter;
	
	list_store=GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(sd->server_list_treeview)));
	gtk_list_store_clear(list_store);
	server_info_list = sd->search_server_list;
	while (server_info_list)
	{
		server = (struct search_server_info*)(server_info_list->data);
		gtk_list_store_append (list_store, &iter);
		gtk_list_store_set (list_store, &iter,
			  	0, server->domain,
				1,server->sessionCount,
				2,server->desc,
				-1);
		server_info_list = g_slist_next(server_info_list);
	}
	gtk_label_set_text(GTK_LABEL(sd->server_list_label),_("Completed"));
}


static void
search_dialog_destroy (GtkWidget * widget, struct search_dialog *sd)
{
	struct myicq_data *md = sd->gc->proto_data;
	search_dialog_clean_search_user_result(sd);
	search_dialog_clean_search_server_result(sd);
	search_dialog_clean_search_group_result(sd);
	g_free(sd);
	md->searchDlg = NULL;
}
*/

SearchDlg::SearchPage SearchDlg::nb_user_list_next ()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_user_list_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		Gtk::TreeModel::Row row = *iter;
		Glib::ustring nick;
		nick = row[m_user_list_columns.nick];
		m_add_friend_nick_label.set_text(nick);
		
		uint8 auth = row[m_user_list_columns.auth];
		
		if (auth == AUTH_ACCEPTED) {
			m_add_friend_info_label.set_text(_("Waiting the add friend reply..."));
			m_add_friend_hbox.hide();
			if (nb_choose_radio == NB_CHOOSE_RADIO_REMOTE_FRIEND)
			{
				/*GtkTreeModel *serv_list_model;
				GtkTreeIter serv_list_iter;	
				if (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (sd->server_list_treeview)), &serv_list_model, &serv_list_iter))
				{
					gchar *domain;
					gchar *uin;
					gchar *bud;
					gtk_tree_model_get (serv_list_model, &serv_list_iter,0,&domain,-1);						
					gtk_tree_model_get (model, &iter,1, &uin,-1);
					bud = g_strdup_printf("%s@%s",uin,domain);
					gtk_label_set_text(GTK_LABEL(sd->add_friend_uin_label),bud);		
					myicq_send_packet_addfriend(sd->gc,bud);
					g_free(domain);
					g_free(uin);
					g_free(bud);
				}*/
			}
			else
			{
				Glib::ustring name;
				name = row[m_user_list_columns.name];
				m_add_friend_uin_label.set_text(name);
				linqMain->getUDPSession()->addContact(name.c_str());
			}
		}
		else if (auth == AUTH_REQUEST) {
			Glib::ustring name;
			name = row[m_user_list_columns.name];
			m_add_friend_uin_label.set_text(name);
			
			m_add_friend_info_label.set_text(_("Authorization is required.\nPlease typing your auth msg."));
			m_add_friend_hbox.set_sensitive(true);
			m_add_friend_hbox.show();
		}
		else {
			m_add_friend_info_label.set_text(_("This contact reject any add friend request."));
			m_add_friend_hbox.hide();
		}
	}
	else {
		Gtk::MessageDialog dialog(*this, _("Please select one user!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
		dialog.run();
		return NB_NULL;
	}
	return NB_ADD_FRIEND;
}

SearchDlg::SearchPage SearchDlg::nb_server_list_next ()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_server_list_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		//Gtk::TreeModel::Row row = *iter;
		/*gchar *domain;
		gtk_tree_model_get (model, &iter,0,&domain,-1);
		search_dialog_clean_search_user_result(sd);
		myicq_send_packet_searchRandom(domain,sd->gc);
		g_free(domain);*/
	}
	else {
		//do_error_dialog(_("Error"), _("Please select one server!"), GAIM_ERROR);
		return NB_NULL;
	}
	return NB_USER_LIST;
}

SearchDlg::SearchPage SearchDlg::nb_group_list_next ()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_group_list_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		//Gtk::TreeModel::Row row = *iter;
		/*uint32 id;
		gtk_label_set_text(GTK_LABEL(sd->create_group_result_info_label),_("Waiting the enter group reply..."));
		gtk_tree_model_get (model, &iter,0,&id,-1);
		myicq_send_packet_enterGroup(id,gtk_entry_get_text(GTK_ENTRY(sd->group_list_password_entry)),sd->gc);*/
		return NB_CREATE_GROUP_RESULT;
	}
	else {
		//do_error_dialog(_("Error"), _("Please select one group!"), GAIM_ERROR);
		return NB_NULL;
	}	
}

SearchDlg::SearchPage SearchDlg::nb_group_type_list_next ()
{
	Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_group_list_treeview.get_selection();
	Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
	if (iter) {
		//Gtk::TreeModel::Row row = *iter;
		if (nb_choose_radio == NB_CHOOSE_RADIO_GROUP_LIST)
		{
			/*gint type;
			gtk_tree_model_get (model, &iter,2,&type,-1);
			search_dialog_clean_search_group_result(sd);
			myicq_send_packet_getGroupList(type,sd->gc);
			*/
			return NB_GROUP_LIST;
		}
		else if (nb_choose_radio == NB_CHOOSE_RADIO_CREATE_GROUP)
		{
			return NB_CREATE_GROUP;
		}
		else
		{
			return NB_CHOOSE;
		}
	}
	else {
		//do_error_dialog(_("Error"), _("Please select one group type!"), GAIM_ERROR);
		return NB_NULL;
	}
}

SearchDlg::SearchPage SearchDlg::nb_create_group_next ()
{
	Glib::ustring text;
	text = m_create_group_name_entry.get_text();
	if (!text.empty()) {
		Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_group_list_treeview.get_selection();
		Gtk::TreeModel::iterator iter = refTreeSelection->get_selected();
		if (iter) {
			//Gtk::TreeModel::Row row = *iter;
			/*uint16 type;
			gchar *name;
			
			gtk_tree_model_get (model, &iter,2,&type,-1);
			gtk_label_set_text(GTK_LABEL(sd->create_group_result_info_label),_("Waiting the create group reply..."));
			name = g_locale_from_utf8(text,-1,NULL,NULL,NULL);
			myicq_send_packet_createGroup(type, name, gtk_entry_get_text(GTK_ENTRY(sd->create_group_password_entry)), sd->gc);*/
			return NB_CREATE_GROUP_RESULT;
		}
		else {
			return NB_CHOOSE;
		}
	}
	else {
		//do_error_dialog(_("Input error"), _("Please input the group name!"), GAIM_ERROR);
		return NB_NULL;
	}
}

SearchDlg::SearchPage SearchDlg::nb_group_num_next ()
{
	uint32 id;
	id = atol(m_group_num_entry.get_text().c_str());
	if (id>0)
	{
		//search_dialog_clean_search_group_result(sd);
		//myicq_send_packet_searchGroup(id,sd->gc);
		return NB_GROUP_LIST;
	}
	else
	{
		//do_error_dialog(_("Input error"), _("Please input the group number!"), GAIM_ERROR);
		return NB_NULL;
	}
}

void SearchDlg::on_next_clicked ()
{
	SearchPage next_page = NB_CHOOSE;
	switch (nb_current_page)
	{
		case NB_CHOOSE:			
			if (m_radio_online_user.get_active()) {
				next_page=NB_USER_LIST;
				nb_choose_radio = NB_CHOOSE_RADIO_RANDOM_SEARCH;				
				clean_search_user_result();
				linqMain->getUDPSession()->searchRandom();
			}
			else if (m_radio_custom_search.get_active()) {
				next_page=NB_CUSTOM_SEARCH;
				nb_choose_radio = NB_CHOOSE_RADIO_CUSTOM_SEARCH;
			}
			else if (m_radio_remote_friend.get_active()) {
				next_page=NB_SERVER_LIST;
				nb_choose_radio = NB_CHOOSE_RADIO_REMOTE_FRIEND;
				//if (!sd->have_server_list)
				//	myicq_send_packet_getServerList(sd->gc);
			}
			else if (m_radio_group_list.get_active()) {
				next_page=NB_GROUP_TYPE_LIST;
				nb_choose_radio = NB_CHOOSE_RADIO_GROUP_LIST;
			}
			else if (m_radio_group_num.get_active()) {
				next_page=NB_GROUP_NUM;
				nb_choose_radio = NB_CHOOSE_RADIO_GROUP_NUM;
			}
			else if (m_radio_create_group.get_active()) {
				next_page=NB_GROUP_TYPE_LIST;
				nb_choose_radio = NB_CHOOSE_RADIO_CREATE_GROUP;
			}
			break;
		case NB_CUSTOM_SEARCH:
			if (m_search_custom_name_entry.get_text().empty() && m_search_custom_nick_entry.get_text().empty() && m_search_custom_email_entry.get_text().empty())
			{
				Gtk::MessageDialog dialog(*this, _("Please input some information to search!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
				dialog.run();
				return;
			}
			else
			{
				clean_search_user_result();
				linqMain->getUDPSession()->searchUser(m_search_custom_name_entry.get_text().c_str(), m_search_custom_nick_entry.get_text().c_str(), m_search_custom_email_entry.get_text().c_str());
				next_page = NB_USER_LIST;
			}
			break;
		case NB_USER_LIST:
			next_page = nb_user_list_next();
			if (next_page == NB_NULL)
				return;
			break;
		case NB_SERVER_LIST:
			next_page = nb_server_list_next();
			if (next_page == NB_NULL)
				return;
			break;
		case NB_GROUP_TYPE_LIST:
			next_page = nb_group_type_list_next();
			if (next_page == NB_NULL)
				return;
			break;
		case NB_GROUP_LIST:
			next_page = nb_group_list_next();
			if (next_page == NB_NULL)
				return;
			break;
		case NB_CREATE_GROUP:
			next_page = nb_create_group_next();
			if (next_page == NB_NULL)
				return;
			break;
		case NB_GROUP_NUM:
			next_page = nb_group_num_next();
			if (next_page == NB_NULL)
				return;
			break;
		default:
			next_page = NB_CHOOSE;
	}
	m_notebook.set_current_page(next_page);
	nb_current_page = next_page;	
	m_back_button.set_sensitive(true);
}

void SearchDlg::on_cancel_clicked ()
{
	delete this;
}

void SearchDlg::create_nb_choose()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Please choose")));
	vbox->pack_start(*frame);
	Gtk::VBox *vbox1;
	vbox1 = Gtk::manage(new Gtk::VBox(false, 6));
	frame->add(*vbox1);
	
	vbox1->pack_start(m_radio_online_user);
	vbox1->pack_start(m_radio_custom_search);
	vbox1->pack_start(m_radio_remote_friend);
	vbox1->pack_start(m_radio_group_list);
	vbox1->pack_start(m_radio_group_num);
	vbox1->pack_start(m_radio_create_group);
	
	Gtk::RadioButton::Group group = m_radio_online_user.get_group();
	m_radio_custom_search.set_group(group);
	m_radio_remote_friend.set_group(group);
	m_radio_group_list.set_group(group);
	m_radio_group_num.set_group(group);
	m_radio_create_group.set_group(group);

	Gtk::Label *label;	
	if (linqMain->getUDPSession()->numClients >0) {
		gchar *text;
		text = g_strdup_printf(_("Online users count:  %u"), linqMain->getUDPSession()->numClients);
		label = Gtk::manage(new Gtk::Label(text));
		g_free(text);
	}
	else
		label = Gtk::manage(new Gtk::Label(_("Online users count:  Unknow")));
	
	label->set_alignment(0, .5);
	vbox->pack_start(*label);

	m_notebook.append_page(*vbox, "");
}

void SearchDlg::create_nb_customSearch()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Notice:\nYou can input one parameter or more to\nsearch the users\n\n")));
	label->set_alignment(0, .5);
	vbox->pack_start(*label);
	
	label = Gtk::manage(new Gtk::Label(_("By user name:")));
	label->set_alignment(0, .5);
	vbox->pack_start(*label);
	m_search_custom_name_entry.signal_activate().connect(SigC::slot(*this, &SearchDlg::on_next_clicked));
	vbox->pack_start(m_search_custom_name_entry);
	label = Gtk::manage(new Gtk::Label(_("By nick name:")));
	label->set_alignment(0, .5);
	vbox->pack_start(*label);
	m_search_custom_nick_entry.signal_activate().connect(SigC::slot(*this, &SearchDlg::on_next_clicked));
	vbox->pack_start(m_search_custom_nick_entry);
	label = Gtk::manage(new Gtk::Label(_("By email:")));
	label->set_alignment(0, .5);
	vbox->pack_start(*label);
	m_search_custom_email_entry.signal_activate().connect(SigC::slot(*this, &SearchDlg::on_next_clicked));
	vbox->pack_start(m_search_custom_email_entry);

	m_notebook.append_page(*vbox, "");
}

void SearchDlg::create_nb_groupNUm()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Notice:\nInput the group number.")));
	label->set_alignment(0, .5);
	vbox->pack_start(*label);
	label = Gtk::manage(new Gtk::Label(_("Group number:")));
	label->set_alignment(0, .5);
	vbox->pack_start(*label);
	m_group_num_entry.signal_activate().connect(SigC::slot(*this, &SearchDlg::on_next_clicked));
	vbox->pack_start(m_group_num_entry);

	m_notebook.append_page(*vbox, "");
}

void SearchDlg::create_nb_createGroup()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Notice:\nInput the group name and password.")));
	label->set_alignment(0, .5);
	vbox->pack_start(*label);

	/*label=gtk_label_new(_("Group name:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	create_group_name_entry=gtk_entry_new();	
	g_signal_connect (G_OBJECT (create_group_name_entry), "activate",
				    G_CALLBACK(on_next_clicked), this);
	gtk_box_pack_start(GTK_BOX(vbox),create_group_name_entry,FALSE,FALSE,0);
	label=gtk_label_new(_("Password:"));
	gtk_misc_set_alignment (GTK_MISC (label), 0, .5);
	gtk_box_pack_start(GTK_BOX(vbox),label,FALSE,FALSE,0);
	create_group_password_entry=gtk_entry_new();	
	g_signal_connect (G_OBJECT (create_group_password_entry), "activate",
				    G_CALLBACK(on_next_clicked), this);
	gtk_box_pack_start(GTK_BOX(vbox),create_group_password_entry,FALSE,FALSE,0);*/

	m_notebook.append_page(*vbox, "");
}

void SearchDlg::create_nb_createGroupResult()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	m_create_group_result_info_label.set_alignment(0, .5);
	vbox->pack_start(m_create_group_result_info_label);

	m_notebook.append_page(*vbox, "");
}

void SearchDlg::on_add_friend_send_button_clicked ()
{
	Glib::ustring reason;
	reason = m_add_friend_textview.get_buffer()->get_text(false);
	Glib::ustring who;
	who = m_add_friend_uin_label.get_text();
	linqMain->getUDPSession()->sendMessage(MSG_AUTH_REQUEST, who.c_str(), reason.c_str());
	m_add_friend_info_label.set_text(_("your request is already send,you can exit now."));
	m_add_friend_hbox.set_sensitive(false);
}

void SearchDlg::create_nb_addFriend()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Add")));
	vbox->pack_start(*frame, true, true, 0);
	Gtk::VBox *vbox1;
	vbox1 = Gtk::manage(new Gtk::VBox(false, 5));
	frame->add(*vbox1);
	vbox1->pack_start(m_add_friend_image, false, false, 0);
	vbox1->pack_start(m_add_friend_uin_label, false, false, 0);
	vbox1->pack_start(m_add_friend_nick_label, false, false, 0);

	frame = Gtk::manage(new Gtk::Frame(_("Please wait")));
	vbox1 = Gtk::manage(new Gtk::VBox(false, 8));
	m_add_friend_info_label.set_alignment(0, .5);
	vbox1->pack_start(m_add_friend_info_label);
	
	m_add_friend_textview.set_size_request(-1, 50);
	//sprintf(str,_("I am %s (%s),can i make friend with you?"),sd->gc->username,sd->gc->displayname);
	//gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(text)),str,-1);

	Gtk::ScrolledWindow *scrolled_window;
	scrolled_window = Gtk::manage(new Gtk::ScrolledWindow());
	scrolled_window->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	scrolled_window->add(m_add_friend_textview);
	scrolled_window->set_shadow_type(Gtk::SHADOW_IN);
	m_add_friend_textview.set_left_margin(2);
	m_add_friend_textview.set_right_margin(2);
	m_add_friend_hbox.pack_start(*scrolled_window, true, true, 0);

	Gtk::Button *button;
	button = Gtk::manage(new Gtk::Button(_("Send")));
	button->signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_add_friend_send_button_clicked));
	m_add_friend_hbox.pack_start(*button, false, false, 0);
	
	vbox1->pack_start(m_add_friend_hbox);

	frame->add(*vbox1);
	vbox->pack_start(*frame);

	m_notebook.append_page(*vbox, "");
}

void SearchDlg::on_user_list_detail_button_clicked ()
{	
/*	GtkTreeModel *model;
	GtkTreeIter iter;
	
	if (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (sd->user_list_treeview)), &model, &iter))
	{
		gchar *bud;
		if (sd->nb_choose_radio == NB_CHOOSE_RADIO_LINQ_USERNAME)
		{
			bud = g_strdup(gtk_entry_get_text(GTK_ENTRY(sd->myicq_num_entry)));
		}
		else if (sd->nb_choose_radio == NB_CHOOSE_RADIO_REMOTE_FRIEND)
		{
			GtkTreeModel *serv_list_model;
			GtkTreeIter serv_list_iter;	
			if (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (sd->server_list_treeview)), &serv_list_model, &serv_list_iter))
			{
				gchar *domain;
				gchar *uin;
				gtk_tree_model_get (serv_list_model, &serv_list_iter,0,&domain,-1);						
				gtk_tree_model_get (model, &iter,1, &uin,-1);
				bud = g_strdup_printf("%s@%s",uin,domain);
				g_free(domain);
				g_free(uin);
			}			
		}
		else			
		{			
			gtk_tree_model_get (model, &iter,1, &bud,-1);
		}
		myicq_get_info(sd->gc,bud);
		g_free(bud);
	}*/
}

bool SearchDlg::on_user_list_button_press(GdkEventButton * event)
{
	if (event->type==GDK_2BUTTON_PRESS)
	{
		on_user_list_detail_button_clicked();
		return true;
	}
	else
	{
		return false;
	}
}

void SearchDlg::on_user_list_previous_button_clicked ()
{
	if (search_user_result_current_page == -1) // all pages
		refresh_search_user_result(0); //show the first page.
	else
		refresh_search_user_result(search_user_result_current_page-1);
}

void SearchDlg::on_user_list_next_button_clicked ()
{
	int total_page;
	total_page = searchUserResultPageList.size();
	if (nb_choose_radio == NB_CHOOSE_RADIO_RANDOM_SEARCH)
	{
		if ((search_user_result_current_page == -1)||(search_user_result_current_page == total_page-1))
		{
			m_user_list_info_label.set_text(_("Searching..."));
			m_user_list_previous_button.set_sensitive(false);
			m_user_list_next_button.set_sensitive(false);
			m_user_list_all_button.set_sensitive(false);
			linqMain->getUDPSession()->searchRandom();
		}		
		else
		{
			refresh_search_user_result(search_user_result_current_page+1);
		}
	}
	else if (nb_choose_radio == NB_CHOOSE_RADIO_REMOTE_FRIEND)
	{
		/*if ((sd->search_user_result_current_page == -1)||(sd->search_user_result_current_page == total_page-1))
		{
			GtkTreeModel *serv_list_model;
			GtkTreeIter serv_list_iter;	
			
			if (gtk_tree_selection_get_selected (gtk_tree_view_get_selection (GTK_TREE_VIEW (sd->server_list_treeview)), &serv_list_model, &serv_list_iter))
			{
				gchar *domain;
				gtk_tree_model_get (serv_list_model, &serv_list_iter,0,&domain,-1);						

				gtk_label_set_text(GTK_LABEL(sd->user_list_info_label),_("Searching..."));
				gtk_widget_set_sensitive(sd->user_list_previous_button,FALSE);
				gtk_widget_set_sensitive(sd->user_list_next_button,FALSE);
				gtk_widget_set_sensitive(sd->user_list_all_button,FALSE);
				myicq_send_packet_searchRandom(domain,sd->gc);	
				g_free(domain);
			}			
		}		
		else
		{
			search_dialog_refresh_search_user_result(sd, sd->search_user_result_current_page+1);
		}*/
	}
	else if (nb_choose_radio == NB_CHOOSE_RADIO_CUSTOM_SEARCH)
	{
		if ((search_user_result_current_page == -1)||(search_user_result_current_page == total_page-1))
		{
			//if (oSearchDlg->customSearch_startUIN!=-1) //if equal -1,then the last search get no result,needn't search again.
			if ((search_user_result_current_page == -1)||(search_user_result_current_page == total_page-1))
			{
				m_user_list_info_label.set_text(_("Searching..."));
				m_user_list_previous_button.set_sensitive(false);
				m_user_list_next_button.set_sensitive(false);
				m_user_list_all_button.set_sensitive(false);
				linqMain->getUDPSession()->searchUser(m_search_custom_name_entry.get_text().c_str(), m_search_custom_nick_entry.get_text().c_str(), m_search_custom_email_entry.get_text().c_str());
			}
			else
			{
				refresh_search_user_result(total_page-1);
			}
		}		
		else
		{
			refresh_search_user_result(search_user_result_current_page+1);
		}
	}
}

void SearchDlg::on_user_list_all_button_clicked ()
{
	refresh_search_user_result(-1); // -1 means all pages.
}

void SearchDlg::create_nb_userList()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));

	m_user_list_liststore = Gtk::ListStore::create(m_user_list_columns);
	m_user_list_treeview.set_model(m_user_list_liststore);
		
	m_user_list_treeview.append_column(_("user name"), m_user_list_columns.name);
	m_user_list_treeview.append_column(_("nick"), m_user_list_columns.nick);
	m_user_list_treeview.append_column(_("status"), m_user_list_columns.status);
	m_user_list_treeview.append_column(_("auth"), m_user_list_columns.auth);
	m_user_list_treeview.append_column(_("gender"), m_user_list_columns.gender);
	m_user_list_treeview.append_column(_("age"), m_user_list_columns.age);

	m_user_list_treeview.signal_button_press_event().connect(SigC::slot(*this, &SearchDlg::on_user_list_button_press));;

	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	sw->add(m_user_list_treeview);

	vbox->pack_start(*sw, true, true, 0);
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 8));
	hbox->pack_start(m_user_list_info_label);
	hbox->pack_start(m_user_list_previous_button);
	m_user_list_previous_button.signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_user_list_previous_button_clicked));
	hbox->pack_start(m_user_list_next_button);
	m_user_list_next_button.signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_user_list_next_button_clicked));
	hbox->pack_start(m_user_list_detail_button);
	m_user_list_detail_button.signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_user_list_detail_button_clicked));
	hbox->pack_start(m_user_list_all_button);
	m_user_list_all_button.signal_clicked().connect(SigC::slot(*this, &SearchDlg::on_user_list_all_button_clicked));

	vbox->pack_start(*hbox, false, false, 0);
	
	m_notebook.append_page(*vbox, "");
}

bool SearchDlg::on_server_list_button_press(GdkEventButton *event)
{
	if (event->type==GDK_2BUTTON_PRESS)
	{
		on_next_clicked();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void SearchDlg::create_nb_serverList()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	
	/*GtkListStore *model;
	model = gtk_list_store_new (3,G_TYPE_STRING,G_TYPE_INT,G_TYPE_STRING);
	server_list_treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	g_object_unref (model);
	
	GtkCellRenderer *renderer;	
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	gint col_offset;
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (server_list_treeview),
							    -1, _("MyICQ Server"),
							    renderer, "text",
							    0,
							    NULL);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (server_list_treeview),
							    -1, _("Online user count"),
							    renderer, "text",
							    1,
							    NULL);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (server_list_treeview),
							    -1, _("Description"),
							    renderer, "text",
							    2,
							    NULL);

	g_signal_connect (G_OBJECT (server_list_treeview),
			"button_press_event",
			G_CALLBACK (on_server_list_button_press),
			this);
	GtkWidget *sw;		
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);	
	gtk_container_add (GTK_CONTAINER (sw), server_list_treeview);
	
	gtk_box_pack_start(GTK_BOX(vbox),sw,TRUE,TRUE,0);
	*/
	
	m_server_list_label.set_text(_("Reading the server list..."));
	vbox->pack_start(m_server_list_label);

	m_notebook.append_page(*vbox, "");
}

bool SearchDlg::on_group_type_list_button_press(GdkEventButton *event)
{
	if (event->type==GDK_2BUTTON_PRESS)
	{
		on_next_clicked();
		return true;
	}
	else
	{
		return false;
	}
}

void SearchDlg::create_nb_groupTypeList()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));

	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Please choose the group type.")));
	vbox->pack_start(*label);
	
	//GtkListStore *model;
	//model = gtk_list_store_new (3,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_INT);
	
	/*grouptype_list = md->servgrouptype;
	GtkTreeIter iter;	
	GSList *grouptype_list;
	struct serv_group_type *grouptype;
	gint type_index = 0;
	while (grouptype_list)
	{
		grouptype = (struct serv_group_type *)(grouptype_list->data);
		gtk_list_store_append (model, &iter);
		gtk_list_store_set (model, &iter,
				0,grouptype->displayname,
				1,grouptype->name,
				2,type_index,
				-1);
		type_index++;
		grouptype_list = g_slist_next(grouptype_list);
	}*/
		
	/*group_type_list_treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	g_object_unref (model);

	GtkCellRenderer *renderer;	
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	gint col_offset;
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (group_type_list_treeview),
							    -1, _("Group Type"),
							    renderer, "text",
							    0,
							    NULL);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (group_type_list_treeview),
							    -1, _("Plug-in"),
							    renderer, "text",
							    1,
							    NULL);

	g_signal_connect (G_OBJECT (group_type_list_treeview),
			"button_press_event",
			G_CALLBACK (on_group_type_list_button_press),
			this);
	GtkWidget *sw;
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);	
	gtk_container_add (GTK_CONTAINER (sw), group_type_list_treeview);
	
	gtk_box_pack_start(GTK_BOX(vbox),sw,TRUE,TRUE,0);
*/
	m_notebook.append_page(*vbox, "");
}

bool SearchDlg::on_group_list_button_press(GdkEventButton *event)
{
	if (event->type==GDK_2BUTTON_PRESS)
	{
		on_next_clicked();
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void SearchDlg::create_nb_groupList()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 8));
	
	/*GtkListStore *model;
	model = gtk_list_store_new (3,G_TYPE_INT,G_TYPE_STRING,G_TYPE_INT); //id,name,num
	group_list_treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	g_object_unref (model);
	
	GtkCellRenderer *renderer;	
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	gint col_offset;
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (group_list_treeview),
							    -1, _("Group ID"),
							    renderer, "text",
							    0,
							    NULL);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (group_list_treeview),
							    -1, _("Group name"),
							    renderer, "text",
							    1,
							    NULL);

	renderer = gtk_cell_renderer_text_new ();
	g_object_set (G_OBJECT (renderer), "xalign", 0.0, NULL);
	col_offset = gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (group_list_treeview),
							    -1, _("Member count"),
							    renderer, "text",
							    2,
							    NULL);

	g_signal_connect (G_OBJECT (group_list_treeview),
			"button_press_event",
			G_CALLBACK (on_group_list_button_press),
			this);
	GtkWidget *sw;
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (sw),GTK_SHADOW_ETCHED_IN);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);	
	gtk_container_add (GTK_CONTAINER (sw), group_list_treeview);
	
	gtk_box_pack_start(GTK_BOX(vbox),sw,TRUE,TRUE,0);

	group_list_info_label= gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(vbox),group_list_info_label,FALSE,FALSE,0);

	GtkWidget *hbox;
	hbox = gtk_hbox_new(FALSE,8);
	GtkWidget *label;
	label= gtk_label_new(_("Password:"));
	gtk_box_pack_start(GTK_BOX(hbox),label,FALSE,FALSE,0);
	group_list_password_entry = gtk_entry_new();
	g_signal_connect (G_OBJECT (group_list_password_entry), "activate",
				    G_CALLBACK(on_next_clicked), this);
	gtk_box_pack_start(GTK_BOX(hbox),group_list_password_entry,FALSE,FALSE,0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE,FALSE,0);
	*/
	m_notebook.append_page(*vbox, "");
}

void SearchDlg::create_notebook()
{
	m_notebook.set_show_tabs(false);
	m_notebook.set_show_border(false);
	create_nb_choose();
	create_nb_customSearch();
	create_nb_userList();
	create_nb_addFriend();
	create_nb_serverList();
	create_nb_groupTypeList();
	create_nb_groupList();
	create_nb_groupNUm();
	create_nb_createGroup();
	create_nb_createGroupResult();
	m_notebook.set_current_page(NB_CHOOSE);
	nb_current_page = NB_CHOOSE;	
}

void SearchDlg::onSearchResult(SEARCH_RESULT result[], int n)
{
	std::list< LinqSearchResult > *resultList;
	resultList = new std::list< LinqSearchResult >;
	for (int i = 0; i < n; i++) {
		resultList->push_back(result[i]);
	}
	searchUserResultPageList.push_back(resultList);
	refresh_search_user_result(searchUserResultPageList.size() -1); //show last page
}

bool SearchDlg::onAddContactReply(const char *name, uint8 auth)
{
	if (((nb_current_page != NB_ADD_FRIEND) && (nb_current_page != NB_USER_LIST)) || (m_add_friend_uin_label.get_text()!= name)) {
		return false;
	}
	char msg[256];
	//switch to result page?
	switch (auth) {
	case AUTH_ACCEPTED:
		g_snprintf(msg,sizeof(msg),_("%s have accepted your add friend request."),name);
		m_add_friend_info_label.set_text(msg);
		
		linqMain->getUDPSession()->getContactInfo(name);
		break;
	case AUTH_REJECTED:
		g_snprintf(msg,sizeof(msg),_("%s have rejected your add friend request."),name);
		m_add_friend_info_label.set_text(msg);
		break;
	case AUTH_REQUEST:
		g_snprintf(msg,sizeof(msg),_("Authorization is required for your previous add friend request.\ntyping your auth msg if you want to add %s to your friend list."),name);
		m_add_friend_info_label.set_text(msg);
		m_add_friend_hbox.set_sensitive(true);
		m_add_friend_hbox.show();
		break;
	}
	return true;
}
