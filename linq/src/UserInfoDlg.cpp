#include "config.h"
#include "UserInfoDlg.h"
#include "LinqMain.h"
#include "intl.h"

#include <gtkmm/separator.h>
#include <gtkmm/frame.h>
#include <gtkmm/table.h>
#include <gtkmm/eventbox.h>
#include <gtkmm/messagedialog.h>

UserInfoDlg::UserInfoDlg(LinqContactInfo &c) : LinqWindow(WIN_VIEW_DETAIL)
{
	name = c.name;
	createWindow(false);
	setContactInfo(c);
	show_all();
}

UserInfoDlg::UserInfoDlg(LinqUserInfo &user) : LinqWindow(WIN_VIEW_DETAIL)
{
	name = linqMain->myInfo.name;
	createWindow(true);
	setUserInfo(user);
	show_all();
}

UserInfoDlg::~UserInfoDlg()
{
}

void UserInfoDlg::onContactInfoReply(LinqContactInfo &c)
{
	setContactInfo(c);
	m_refresh_button.set_sensitive(true);
}

void UserInfoDlg::onUserInfoReply(LinqUserInfo &user)
{
	setUserInfo(user);
	m_refresh_button.set_sensitive(true);
}

void UserInfoDlg::createWindow(bool is_myself)
{
	signal_delete_event().connect(SigC::slot(*this, &UserInfoDlg::on_window_delete_event));
	set_resizable(false);
	if (is_myself)
		set_title(_("View my information"));
	else
		set_title(_("View contact's information"));
	set_border_width(5);

	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	add(*vbox);
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 18));	
	vbox->pack_start(*hbox);
	hbox->set_border_width(10);
	
	hbox->pack_start(createLeftTree(is_myself), false, false, 0);
	
	hbox->pack_start(m_notebook, true, true, 0);
	m_notebook.set_show_tabs(false);
	m_notebook.set_show_border(false);

	m_notebook.append_page(createPageBasic(is_myself), "");
	m_notebook.append_page(createPageContact(is_myself), "");
	m_notebook.append_page(createPageDetail(is_myself), "");
	if (is_myself)
		m_notebook.append_page(createPageMyinfo(), "");
	
	Gtk::HSeparator *sep;
	sep = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*sep);
	
	hbox = Gtk::manage(new Gtk::HBox(false, 5));
	vbox->pack_start(*hbox);
	m_refresh_button.set_label(_("Refresh"));
	m_refresh_button.signal_clicked().connect(SigC::slot(*this, &UserInfoDlg::on_refresh_button_clicked));
	hbox->pack_start(m_refresh_button);
	m_cancel_button.set_label(_("Cancel"));
	m_cancel_button.signal_clicked().connect(SigC::slot(*this, &UserInfoDlg::on_close_button_clicked));
	hbox->pack_end(m_cancel_button);
	m_ok_button.set_label(_("Ok"));
	m_ok_button.signal_clicked().connect(SigC::slot(*this, &UserInfoDlg::on_change_button_clicked));
	hbox->pack_end(m_ok_button);
}

void UserInfoDlg::setContactInfo(LinqContactInfo &c)
{
	//m_refresh_button[0].set_sensitive(true);
	//m_refresh_button[1].set_sensitive(true);
	//m_refresh_button[2].set_sensitive(true);

	if (!c.name.empty())
		m_name_entry.set_text(c.name);
	else
		m_name_entry.set_text(linqMain->myInfo.name.c_str());
	m_nick_entry.set_text(c.nick);
	
	if (age_entry) {
		gchar *age = g_strdup_printf("%d", (int)((time(NULL) - c.birth) / (3600 * 24 * 365)));
		age_entry->set_text(age);
		g_free(age);
	}
	else {
		struct tm *tm = localtime(&((time_t)(c.birth)));
		gchar *year = g_strdup_printf("%d", tm->tm_year + 1900);
		gchar *month = g_strdup_printf("%d", tm->tm_mon + 1);
		gchar *day = g_strdup_printf("%d", tm->tm_mday);
		birth_year_entry->set_text(year);
		birth_month_entry->set_text(month);
		birth_day_entry->set_text(day);
		g_free(year);
		g_free(month);
		g_free(day);
	}
	
	m_gender_optionmenu.set_history(c.gender);
	m_country_combo.get_entry()->set_text(c.country);
	m_city_entry.set_text(c.city);
	m_email_entry.set_text(c.email);
	m_address_entry.set_text(c.address);
	m_postcode_entry.set_text(c.postcode);
	m_tel_entry.set_text(c.tel);
	m_mobile_entry.set_text(c.mobile);
	m_realname_entry.set_text(c.realname);
	m_occupation_combo.get_entry()->set_text(c.occupation);
	m_homepage_entry.set_text(c.homepage);
	m_intro_textview.get_buffer()->set_text(c.intro);
}

void UserInfoDlg::setUserInfo(LinqUserInfo &user)
{
	setContactInfo((LinqContactInfo &)user);
	
	//m_refresh_button[3].set_sensitive(true);
	
	if (user.auth == AUTH_ACCEPTED)
		auth_radio_1->set_active(true);
	else if (user.auth == AUTH_REQUEST)
		auth_radio_2->set_active(true);
	else // AUTH_REJECTED
		auth_radio_3->set_active(true);
}

void UserInfoDlg::get_list_by_str(std::list<std::string> &gl, const char *str)
{
	gl.clear();
	char *mystr = g_strdup(str);
	char *a,*b;
	b = mystr;
	a = strchr(mystr,'\n');
	while (a)
	{
		*a = '\0';
		gl.push_back(b);
		b = a+1;
		a = strchr(b,'\n');
	}
	gl.push_back(b);
	g_free(mystr);
}

Gtk::Widget &UserInfoDlg::createLeftTree(bool is_myself)
{
	Glib::RefPtr<Gtk::TreeStore> treestore;
	treestore = Gtk::TreeStore::create(m_Columns);

	Gtk::TreeModel::iterator iter = treestore->append();
	Gtk::TreeModel::Row row = *iter;	
	row[m_Columns.text] = _("Basic information");
	row[m_Columns.page] = 0;

	iter = treestore->append();
	row = *iter;	
	row[m_Columns.text] = _("Contact method");
	row[m_Columns.page] = 1;

	iter = treestore->append();
	row = *iter;	
	row[m_Columns.text] = _("Detail information");
	row[m_Columns.page] = 2;

	if (is_myself) {
		iter = treestore->append();
		row = *iter;	
		row[m_Columns.text] = _("Network safety");
		row[m_Columns.page] = 3;
	}
	
	m_treeview.set_headers_visible(false);
	m_treeview.set_rules_hint(true);
	
	m_treeview.set_model(treestore);
	
	m_treeview.append_column("", m_Columns.text);
	m_treeview.get_selection()->signal_changed().connect(SigC::slot(*this, &UserInfoDlg::on_treeview_selection_changed));
	
	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_shadow_type(Gtk::SHADOW_ETCHED_IN);
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	sw->set_size_request (120, 180);
	
	sw->add(m_treeview);
	return *sw;
}

Gtk::Widget &UserInfoDlg::createPageBasic(bool is_myself)
{
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Basic information")));
	frame->set_border_width(5);
	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(7, 3, false));
	table->set_border_width(5);
	frame->add(*table);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Head icon:")));
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 0, 0);
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(linqMain->skin->linq.face[0]));
	if (is_myself) {
		Gtk::EventBox *event_box;
		event_box = Gtk::manage(new Gtk::EventBox());
		event_box->add(*image);
		table->attach(*event_box, 0, 1, 1, 3, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 0, 0);
		//g_object_set_data(G_OBJECT(updateinfo->face),"user_data",GINT_TO_POINTER((gint)info->face));	
		/*g_signal_connect (G_OBJECT (event_box), "button_press_event",
			    G_CALLBACK(on_window_change_face), updateinfo);*/
	}
	else {
		table->attach(*image, 0, 1, 1, 3, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 0, 0);
	}
	label = Gtk::manage(new Gtk::Label(_("User name:")));
	label->set_alignment(1, .5);
	table->attach(*label, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 0, 0);
	label = Gtk::manage(new Gtk::Label(_("User nick:")));
	label->set_alignment(1, .5);
	table->attach(*label, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 0, 0);
	
	m_name_entry.set_size_request(120, -1);
	table->attach(m_name_entry, 2, 3, 1, 2, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);
	m_nick_entry.set_size_request(120, -1);
	table->attach(m_nick_entry, 2, 3, 2, 3, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);

	if (is_myself) {
		label = Gtk::manage(new Gtk::Label(_("Birth")));
		label->set_alignment(0, .5);
		table->attach(*label, 0, 2, 3, 4, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);		

		Gtk::HBox *hbox;
		hbox = Gtk::manage(new Gtk::HBox(false, 2));
		table->attach(*hbox, 0, 2, 4, 5, Gtk::FILL, Gtk::FILL, 2, 0);
		
		birth_year_entry = Gtk::manage(new Gtk::Entry());
		birth_year_entry->set_width_chars (4);
		hbox->pack_start(*birth_year_entry, false, false, 0);
		birth_month_entry = Gtk::manage(new Gtk::Entry());
		birth_month_entry->set_width_chars (2);
		hbox->pack_start(*birth_month_entry, false, false, 0);
		birth_day_entry = Gtk::manage(new Gtk::Entry());
		birth_day_entry->set_width_chars (2);
		hbox->pack_start(*birth_day_entry, false, false, 0);		
		
		age_entry = NULL;
	}
	else {
		label = Gtk::manage(new Gtk::Label(_("Age")));
		label->set_alignment(0, .5);
		table->attach(*label, 0, 2, 3, 4, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);		
		
		age_entry = Gtk::manage(new Gtk::Entry());
		age_entry->set_size_request (150, -1);
		table->attach(*age_entry, 0, 2, 4, 5, Gtk::FILL, Gtk::FILL, 2, 0);
	}
	label = Gtk::manage(new Gtk::Label(_("Gender")));
	label->set_alignment(0, .5);
	table->attach(*label, 2, 3, 3, 4, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);
	
	Gtk::Menu *menu;
	menu = Gtk::manage(new Gtk::Menu());
	Gtk::Menu::MenuList& menulist = menu->items();
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Unspecified")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Male")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Female")));
	m_gender_optionmenu.set_menu(*menu);
	m_gender_optionmenu.set_size_request (120, -1);
	table->attach(m_gender_optionmenu, 2, 3, 4, 5, Gtk::FILL, Gtk::FILL, 2, 0);

	label = Gtk::manage(new Gtk::Label(_("Country/Region")));
	label->set_alignment(0, .5);
	table->attach(*label, 0, 2, 5, 6, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);
	label = Gtk::manage(new Gtk::Label(_("City")));
	label->set_alignment(0, .5);
	table->attach(*label, 2, 3, 5, 6, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);

	std::list<std::string> gl;
	get_list_by_str(gl, _("America\nBelgium\nChina\nEngland\nFrance\nGermany\nJapan"));
	m_country_combo.set_popdown_strings(gl);
	m_country_combo.set_size_request (150, -1);
	table->attach(m_country_combo, 0, 2, 6, 7, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);
	
	m_city_entry.set_size_request (120, -1);
	table->attach(m_city_entry, 2, 3, 6, 7, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);
	
	return *frame;
}

Gtk::Widget &UserInfoDlg::createPageContact(bool is_myself)
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Connection method")));
	vbox->pack_start(*frame, true, true, 0);	
	frame->set_border_width(5);

	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(2, 5, false));
	table->set_border_width(5);
	frame->add(*table);

	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Email:")));	
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL, 0, 0);
	label = Gtk::manage(new Gtk::Label(_("Address:")));	
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL, 0, 0);
	label = Gtk::manage(new Gtk::Label(_("Post code:")));	
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 2, 3, Gtk::FILL, Gtk::FILL, 0, 0);
	label = Gtk::manage(new Gtk::Label(_("Telphone:")));	
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 3, 4, Gtk::FILL, Gtk::FILL, 0, 0);
	label = Gtk::manage(new Gtk::Label(_("Mobile:")));	
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 4, 5, Gtk::FILL, Gtk::FILL, 0, 0);

	table->attach(m_email_entry, 1, 2, 0, 1, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 0, 0);
	table->attach(m_address_entry, 1, 2, 1, 2, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 0, 2);
	table->attach(m_postcode_entry, 1, 2, 2, 3, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 0, 0);
	table->attach(m_tel_entry, 1, 2, 3, 4, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 0, 2);
	table->attach(m_mobile_entry, 1, 2, 4, 5, Gtk::FILL | Gtk::EXPAND, Gtk::FILL, 0, 0);

	return *vbox;
}

Gtk::Widget &UserInfoDlg::createPageDetail(bool is_myself)
{
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Detail information")));
	frame->set_border_width(5);

	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(6, 2, false));
	table->set_border_width(5);
	frame->add(*table);

	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Real name")));
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 0, 1, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);
	label = Gtk::manage(new Gtk::Label(_("Occupation")));
	label->set_alignment(0, .5);
	table->attach(*label, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);

	m_realname_entry.set_size_request(120, -1);
	table->attach(m_realname_entry, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);
	
	m_occupation_combo.set_size_request(90, -1);
	std::list<std::string> gl;
	get_list_by_str(gl, _("occupation\nlist"));
	m_country_combo.set_popdown_strings(gl);
	table->attach(m_occupation_combo, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);

	label = Gtk::manage(new Gtk::Label(_("Home page:")));
	label->set_alignment(0, .5);
	table->attach(*label, 0, 2, 2, 3, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 0, 2);

	table->attach(m_homepage_entry, 0, 2, 3, 4, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 0, 0);

	label = Gtk::manage(new Gtk::Label(_("Introduce:")));
	label->set_alignment(0, .5);
	table->attach(*label, 0, 2, 4, 5, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 0, 2);

	m_intro_textview.set_size_request(-1, 50);
	m_intro_textview.set_left_margin(2);
	m_intro_textview.set_right_margin(2);
	
	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
	sw->set_shadow_type(Gtk::SHADOW_IN);
	sw->add(m_intro_textview);

	table->attach(*sw, 0, 2, 5, 6, Gtk::FILL | Gtk::EXPAND, Gtk::FILL | Gtk::EXPAND, 0, 0);
	
	return *frame;
}

void UserInfoDlg::on_change_passwd_checkbutton_toggled()
{
	if (change_passwd_check_button->get_active()) {
		old_passwd_entry->set_sensitive(true);
		new_passwd_entry_1->set_sensitive(true);
		new_passwd_entry_2->set_sensitive(true);
	}
	else {
		old_passwd_entry->set_sensitive(false);
		new_passwd_entry_1->set_sensitive(false);
		new_passwd_entry_2->set_sensitive(false);
	}
}

Gtk::Widget &UserInfoDlg::createPageMyinfo()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame(_("Change password")));
	frame->set_border_width(5);
	vbox->pack_start(*frame, false, false, 0);
	Gtk::Table *table;
	table = Gtk::manage(new Gtk::Table(2, 4, false));
	table->set_border_width(5);
	frame->add(*table);
	Gtk::Label *label;
	label = Gtk::manage(new Gtk::Label(_("Old password:")));
	label->set_alignment(0, .5);
	table->attach(*label, 1, 2, 0, 1, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);
	label = Gtk::manage(new Gtk::Label(_("New password:")));
	label->set_alignment(0, .5);
	table->attach(*label, 0, 1, 2, 3, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);
	label = Gtk::manage(new Gtk::Label(_("Password confirm:")));
	label->set_alignment(0, .5);
	table->attach(*label, 1, 2, 2, 3, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);

	change_passwd_check_button = Gtk::manage(new Gtk::CheckButton(_("Change password")));
	change_passwd_check_button->signal_toggled().connect(SigC::slot(*this, &UserInfoDlg::on_change_passwd_checkbutton_toggled));
	table->attach(*change_passwd_check_button, 0, 1, 1, 2, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);

	old_passwd_entry = Gtk::manage(new Gtk::Entry());
	old_passwd_entry->set_visibility(false);
	old_passwd_entry->set_sensitive(false);
	old_passwd_entry->set_size_request(120, -1);
	table->attach(*old_passwd_entry, 1, 2, 1, 2, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 0);

	new_passwd_entry_1 = Gtk::manage(new Gtk::Entry());
	new_passwd_entry_1->set_visibility(false);
	new_passwd_entry_1->set_sensitive(false);
	new_passwd_entry_1->set_size_request(120, -1);	
	table->attach(*new_passwd_entry_1, 0, 1, 3, 4, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);

	new_passwd_entry_2 = Gtk::manage(new Gtk::Entry());
	new_passwd_entry_2->set_visibility(false);
	new_passwd_entry_2->set_sensitive(false);
	new_passwd_entry_2->set_size_request(120, -1);	
	table->attach(*new_passwd_entry_2, 1, 2, 3, 4, Gtk::FILL, Gtk::FILL | Gtk::EXPAND, 2, 2);

	frame = Gtk::manage(new Gtk::Frame(_("Authorization")));
	frame->set_border_width(5);
	vbox->pack_start(*frame, false, false, 0);

	Gtk::VBox *vbox1;
	vbox1 = Gtk::manage(new Gtk::VBox(false, 1));
	frame->add(*vbox1);
	
	auth_radio_1 = Gtk::manage(new Gtk::RadioButton(_("anyone can add me to their contact lists")));
	vbox1->pack_start(*auth_radio_1, false, false, 0);
	auth_radio_2 = Gtk::manage(new Gtk::RadioButton(_("authorization is required")));
	vbox1->pack_start(*auth_radio_2, false, false, 0);
	auth_radio_3 = Gtk::manage(new Gtk::RadioButton(_("none can add me to their contact lists")));
	vbox1->pack_start(*auth_radio_3, false, false, 0);
	Gtk::RadioButton::Group group = auth_radio_1->get_group();
	auth_radio_2->set_group(group);
	auth_radio_3->set_group(group);

	return *vbox;
}

bool UserInfoDlg::on_window_delete_event(GdkEventAny * event)
{
	delete this;
	return true;
}

void UserInfoDlg::on_treeview_selection_changed()
{
	Gtk::TreeModel::iterator iter = m_treeview.get_selection()->get_selected();
	if (iter) {
  		Gtk::TreeModel::Row row = *iter;
		m_notebook.set_current_page(row[m_Columns.page]);
	}
}

void UserInfoDlg::getUserInfo(LinqUserInfo &info)
{
	info.name = m_name_entry.get_text(); //needn't set in fact.
	info.nick = m_nick_entry.get_text();
	info.gender = m_gender_optionmenu.get_history();
	
	struct tm birth_tm;
	birth_tm.tm_sec = 0;
	birth_tm.tm_min = 0;
	birth_tm.tm_hour = 0;
	birth_tm.tm_mday = atoi(birth_day_entry->get_text().c_str());
	birth_tm.tm_mon = atoi(birth_month_entry->get_text().c_str()) -1;
	birth_tm.tm_year = atoi(birth_year_entry->get_text().c_str()) - 1900;
	birth_tm.tm_wday = 0;
	birth_tm.tm_yday = 0;
	birth_tm.tm_isdst = 0;
			
	info.birth = mktime(&birth_tm);

	info.email = m_email_entry.get_text();
	info.country = m_country_combo.get_entry()->get_text();
	info.city = m_city_entry.get_text();
	info.address = m_address_entry.get_text();
	info.postcode = m_postcode_entry.get_text();
	info.tel = m_tel_entry.get_text();
	info.mobile = m_mobile_entry.get_text();
	info.realname = m_realname_entry.get_text();
	info.occupation = m_occupation_combo.get_entry()->get_text();
	info.homepage = m_homepage_entry.get_text();
			
	info.intro = m_intro_textview.get_buffer()->get_text(false);
	
	if (auth_radio_1->get_active())
		info.auth = AUTH_ACCEPTED;
	else if (auth_radio_2->get_active())
		info.auth = AUTH_REQUEST;
	else
		info.auth = AUTH_REJECTED;
}

void UserInfoDlg::on_change_button_clicked()
{
	if (name == linqMain->myInfo.name) {
		if (change_passwd_check_button->get_active()) {
			if (old_passwd_entry->get_text() != linqMain->myInfo.passwd) {
				Gtk::MessageDialog dialog(*this, _("password is wrong!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
				dialog.run();
				return;
			}
			Glib::ustring passwd = new_passwd_entry_1->get_text();
			if (passwd != new_passwd_entry_2->get_text()) {
				Gtk::MessageDialog dialog(*this, _("new password don't equal!"), Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
				dialog.run();
				return;
			}
			linqMain->getUDPSession()->changePasswd(passwd.c_str());			
		}
		LinqUserInfo nowuserInfo;
		LinqDB::loadUserInfo(nowuserInfo);
		
		LinqUserInfo userInfo;
		getUserInfo(userInfo);
		if (!(nowuserInfo == userInfo)) {
			g_print("infochanged\n");
			
			LinqDB::saveUserInfo(userInfo);

			USER_INFO info;			
			info.name = userInfo.name.c_str();
			info.nick = userInfo.nick.c_str();
			info.gender = userInfo.gender;
			info.birth = userInfo.birth;
			info.email = userInfo.email.c_str();
			info.country = userInfo.country.c_str();
			info.city = userInfo.city.c_str();
			info.address = userInfo.address.c_str();
			info.postcode = userInfo.postcode.c_str();
			info.tel = userInfo.tel.c_str();
			info.mobile = userInfo.mobile.c_str();
			info.realname = userInfo.realname.c_str();
			info.occupation = userInfo.occupation.c_str();
			info.homepage = userInfo.homepage.c_str();
			info.intro = userInfo.intro.c_str();
			info.auth = userInfo.auth;			

			linqMain->getUDPSession()->updateUserInfo(info);
		}
	}
	delete this;
}

void UserInfoDlg::on_refresh_button_clicked()
{
	if (name == linqMain->myInfo.name) {
		linqMain->getUDPSession()->getUserInfo();
	}
	else {
		linqMain->getUDPSession()->getContactInfo(name.c_str());		
	}
	m_refresh_button.set_sensitive(false);
}

void UserInfoDlg::on_close_button_clicked()
{
	delete this;
}
