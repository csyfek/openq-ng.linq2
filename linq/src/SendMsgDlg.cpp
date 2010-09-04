#include "config.h"
#include "SendMsgDlg.h"
#include "LinqMain.h"
#include "gtkimhtml.h"
#include "intl.h"

#include <gtkmm/menubar.h>
#include <gtkmm/frame.h>
#include <gtkmm/table.h>
#include <gtkmm/stock.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/separator.h>

SendMsgDlg::SendMsgDlg(const char *name, time_t when, const char *text)
: LinqWindow(WIN_SEND_MESSAGE)
{
	signal_delete_event().connect(SigC::slot(*this, &SendMsgDlg::on_window_delete_event));
	set_title (_("LinQ - Send message"));

	m_notebook.set_scrollable(true);
	m_notebook.popup_enable();

	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	
	Gtk::MenuBar *menubar;
	menubar = Gtk::manage(new Gtk::MenuBar());

	Gtk::Menu *menu;
	menu = Gtk::manage(new Gtk::Menu());	
	Gtk::Menu::MenuList& menulist = menu->items();
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("View _histroy")));
	menulist.push_back(Gtk::Menu_Helpers::MenuElem(_("_Close")));	
	menubar->items().push_back( Gtk::Menu_Helpers::MenuElem("_Conservation", *menu) );
	
	menu = Gtk::manage(new Gtk::Menu());	
	menulist = menu->items();
	menulist.push_back(Gtk::Menu_Helpers::CheckMenuElem(_("Enabel _sound")));
	menubar->items().push_back( Gtk::Menu_Helpers::MenuElem("_Conservation", *menu) );

	vbox->pack_start(*menubar, false, true, 0);
	vbox->pack_start(m_notebook, true, true, 0);

	add(*vbox);

	SendMsgDlgTab *tab;
	tab = new SendMsgDlgTab(this, name);
	if (text)
		tab->onRecvMessage(when, text);
	tab->doPresent();

	show_all();
}

SendMsgDlg::~SendMsgDlg()
{
	while (!tabList.empty())
		delete (SendMsgDlgTab *) tabList.front(); //it will remove from tabList when be deleted.
}

void SendMsgDlg::onSendMessage(const char *name)
{
	bool found = false;
	std::list<SendMsgDlgTab *>::iterator iter;
	for (iter = tabList.begin(); iter!= tabList.end(); ++iter) {
		if ((*iter)->getName() == name) {			
			(*iter)->doPresent();
			found = true;
			break;
		}
	}
	if (!found) {
		SendMsgDlgTab *tab;
		tab = new SendMsgDlgTab(this, name);
		tab->doPresent();
	}
	present();
}

void SendMsgDlg::onRecvMessage(const char *name, time_t when, const char *text)
{
	bool found = false;
	std::list<SendMsgDlgTab *>::iterator iter;
	for (iter = tabList.begin(); iter!= tabList.end(); ++iter) {
		if ((*iter)->getName() == name) {			
			(*iter)->onRecvMessage(when, text);
			(*iter)->doActive();
			found = true;
			break;
		}
	}
	if (!found) {
		SendMsgDlgTab *tab;
		tab = new SendMsgDlgTab(this, name);
		tab->onRecvMessage(when, text);
		tab->doActive();
	}
	present();
}

bool SendMsgDlg::on_window_delete_event(GdkEventAny * event)
{
	delete this;
	return true;
}

SendMsgDlgTab::SendMsgDlgTab(SendMsgDlg *oSendMsgDlg, const char *name)
: Gtk::VBox(false, 5),
  m_tab_label(name)
{
	this->oSendMsgDlg = oSendMsgDlg;
	this->name = name;
	
	oSendMsgDlg->tabList.push_back(this);

	Gtk::VPaned *paned;
	paned = setup_im_pane();

	set_border_width(5);
	add(*paned);

	Gtk::HBox *tab_hbox;
	tab_hbox = Gtk::manage(new Gtk::HBox(false, 5));
	Gtk::Button *close_button;
	close_button = Gtk::manage(new Gtk::Button());
	close_button->set_size_request(16, 16);
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::CLOSE, Gtk::ICON_SIZE_MENU));
	close_button->add(*image);
	close_button->set_relief(Gtk::RELIEF_NONE);
	close_button->signal_clicked().connect(SigC::slot(*this, &SendMsgDlgTab::on_close_button_clicked));
	//gtk_tooltips_set_tip(gtkconv->tooltips, gtkconv->close, _("Close conversation"), NULL);

	m_tab_label.set_use_markup(true);
	tab_hbox->pack_start(m_tab_label, true, true, 0);
	tab_hbox->pack_start(*close_button, false, false, 0);
	tab_hbox->show_all();
	
	oSendMsgDlg->m_notebook.append_page(*this, *tab_hbox);
	oSendMsgDlg->m_notebook.set_menu_label_text(*this, name);
	
	show_all();
}

Gtk::VPaned *SendMsgDlgTab::setup_im_pane()
{
	Gtk::VPaned *paned;
	paned = Gtk::manage(new Gtk::VPaned);
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 5));
	paned->pack1(*vbox, true, true);
	
	Gtk::ScrolledWindow *sw;
	sw = Gtk::manage(new Gtk::ScrolledWindow());
	sw->set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_ALWAYS);
	sw->set_shadow_type(Gtk::SHADOW_IN);
	vbox->pack_start(*sw, true, true, 0);
	sw->set_size_request(230, 150);
	
	imhtml = gtk_imhtml_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(sw->gobj()), imhtml);
	gtk_imhtml_show_comments(GTK_IMHTML(imhtml), true);
	
	Gtk::VBox *vbox2;
	vbox2 = Gtk::manage(new Gtk::VBox(false, 5));
	paned->pack2(*vbox2, false, false);
	
	Gtk::VBox *tool_vbox;
	tool_vbox = build_conv_toolbutton_vbox();
	
	vbox2->pack_start(*tool_vbox, false, false, 0);
	
	Gtk::Frame *frame;
	frame = Gtk::manage(new Gtk::Frame());
	frame->set_shadow_type(Gtk::SHADOW_IN);
	vbox2->pack_start(*frame, true, true, 0);
	
	m_input_textview.set_wrap_mode(Gtk::WRAP_WORD);
	m_input_textview.set_size_request(-1, 45);
	frame->add(m_input_textview);
	//m_input_textview.grab_focus();
	m_input_textview.signal_event().connect(SigC::slot(*this, &SendMsgDlgTab::on_input_textview_event));

	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 5));
	vbox2->pack_start(*hbox, false, false, 0);
	setup_im_buttons(hbox);
	
	return paned;
}

Gtk::VBox *SendMsgDlgTab::build_conv_toolbutton_vbox()
{
	Gtk::VBox *vbox;
	vbox = Gtk::manage(new Gtk::VBox(false, 0));
	Gtk::HSeparator *hsep;
	hsep = Gtk::manage(new Gtk::HSeparator());
	vbox->pack_start(*hsep, false, false, 0);
	
	Gtk::HBox *hbox;
	hbox = Gtk::manage(new Gtk::HBox(false, 5));
	vbox->pack_start(*hbox, false, false, 0);
	
	m_bold_button.set_relief(Gtk::RELIEF_NONE);
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::BOLD, Gtk::ICON_SIZE_MENU));
	m_bold_button.add(*image);
	hbox->pack_start(m_bold_button, false, false, 0);
	m_bold_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_bold_button_toggled));

	m_italic_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::ITALIC, Gtk::ICON_SIZE_MENU));
	m_italic_button.add(*image);
	hbox->pack_start(m_italic_button, false, false, 0);
	m_italic_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_italic_button_toggled));

	m_underline_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::UNDERLINE, Gtk::ICON_SIZE_MENU));
	m_underline_button.add(*image);
	hbox->pack_start(m_underline_button, false, false, 0);
	m_underline_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_underline_button_toggled));

	Gtk::VSeparator *vsep;	
	vsep = Gtk::manage(new Gtk::VSeparator());
	hbox->pack_start(*vsep, false, false, 0);

	m_bigfont_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_MENU));
	m_bigfont_button.add(*image);
	hbox->pack_start(m_bigfont_button, false, false, 0);
	m_bigfont_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_bigfont_button_toggled));

	m_normalfont_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_MENU));
	m_normalfont_button.add(*image);
	hbox->pack_start(m_normalfont_button, false, false, 0);
	m_normalfont_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_normalfont_button_toggled));

	m_smallfont_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_MENU));
	m_smallfont_button.add(*image);
	hbox->pack_start(m_smallfont_button, false, false, 0);
	m_smallfont_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_smallfont_button_toggled));

	vsep = Gtk::manage(new Gtk::VSeparator());
	hbox->pack_start(*vsep, false, false, 0);

	m_selectfont_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_MENU));
	m_selectfont_button.add(*image);
	hbox->pack_start(m_selectfont_button, false, false, 0);
	m_selectfont_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_selectfont_button_toggled));

	m_fgcolor_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_MENU));
	m_fgcolor_button.add(*image);
	hbox->pack_start(m_fgcolor_button, false, false, 0);
	m_fgcolor_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_fgcolor_button_toggled));

	m_bgcolor_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_MENU));
	m_bgcolor_button.add(*image);
	hbox->pack_start(m_bgcolor_button, false, false, 0);
	m_bgcolor_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_bgcolor_button_toggled));

	vsep = Gtk::manage(new Gtk::VSeparator());
	hbox->pack_start(*vsep, false, false, 0);

	m_smilies_button.set_relief(Gtk::RELIEF_NONE);
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_FONT, Gtk::ICON_SIZE_MENU));
	m_smilies_button.add(*image);
	hbox->pack_start(m_smilies_button, false, false, 0);
	m_smilies_button.signal_toggled().connect(SigC::slot(*this, &SendMsgDlgTab::on_bgcolor_button_toggled));

	vsep = Gtk::manage(new Gtk::VSeparator());
	hbox->pack_start(*vsep, false, false, 0);
	
	return vbox;
}

void SendMsgDlgTab::setup_im_buttons(Gtk::HBox *hbox)
{
	m_send_button.set_relief(Gtk::RELIEF_NONE);	
	Gtk::Image *image;
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::CONVERT, Gtk::ICON_SIZE_BUTTON));
	m_send_button.add(*image);
	hbox->pack_end(m_send_button, false, false, 0);
	m_send_button.signal_clicked().connect(SigC::slot(*this, &SendMsgDlgTab::on_send_button_clicked));

	Gtk::VSeparator *vsep;	
	vsep = Gtk::manage(new Gtk::VSeparator());
	hbox->pack_end(*vsep, false, true, 0);

	m_warning_button.set_relief(Gtk::RELIEF_NONE);	
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::DIALOG_WARNING, Gtk::ICON_SIZE_BUTTON));
	m_warning_button.add(*image);
	hbox->pack_start(m_warning_button, false, false, 0);
	m_warning_button.signal_clicked().connect(SigC::slot(*this, &SendMsgDlgTab::on_warning_button_clicked));

	m_info_button.set_relief(Gtk::RELIEF_NONE);	
	image = Gtk::manage(new Gtk::Image(Gtk::Stock::FIND, Gtk::ICON_SIZE_BUTTON));
	m_info_button.add(*image);
	hbox->pack_start(m_info_button, false, false, 0);
	m_info_button.signal_clicked().connect(SigC::slot(*this, &SendMsgDlgTab::on_info_button_clicked));
}

SendMsgDlgTab::~SendMsgDlgTab()
{	
	oSendMsgDlg->tabList.remove(this);
}

void SendMsgDlgTab::onRecvMessage(time_t when, const char *text)
{
	char mdate[64];
	strftime(mdate, sizeof(mdate), "%H:%M:%S", localtime(&when));
	std::string str;
	str = "<FONT SIZE=\"2\">(";
	str += mdate;
	str += ") </FONT><B>";
	str += name;
	str += ": ";
	str += text;
	str += "</B><BR>";
	
	gtk_imhtml_append_text(GTK_IMHTML(imhtml), str.c_str(), str.length(), (GtkIMHtmlOptions)0);
}

void SendMsgDlgTab::doPresent()
{
	if (oSendMsgDlg->m_notebook.get_nth_page(oSendMsgDlg->m_notebook.get_current_page()) != this) {
		int num = oSendMsgDlg->m_notebook.get_n_pages();
		for (int i=num-1;i>=0;i--) {
			if (oSendMsgDlg->m_notebook.get_nth_page(i) == this) {
				oSendMsgDlg->m_notebook.set_current_page(i);
				break;
			}
		}
	}
	m_input_textview.grab_focus();
}

void SendMsgDlgTab::doActive()
{
	if (oSendMsgDlg->m_notebook.get_nth_page(oSendMsgDlg->m_notebook.get_current_page()) == this) {
		m_input_textview.grab_focus();
		return;
	}

	Glib::ustring str = "<span foreground=\"red\">" + name + "</span>";
	m_tab_label.set_markup(str);
}

void SendMsgDlgTab::on_close_button_clicked()
{
	delete this;
}

void SendMsgDlgTab::on_bold_button_toggled()
{
	if (m_bold_button.get_active())
		doSurround("<B>", "</B>");
	else
		doAdvancePast("<B>", "</B>");
}

void SendMsgDlgTab::on_italic_button_toggled()
{
	if (m_italic_button.get_active())
		doSurround("<I>", "</I>");
	else
		doAdvancePast("<I>", "</I>");
}

void SendMsgDlgTab::on_underline_button_toggled()
{
	if (m_bold_button.get_active())
		doSurround("<U>", "</U>");
	else
		doAdvancePast("<U>", "</U>");
}

void SendMsgDlgTab::on_bigfont_button_toggled()
{
	if (m_bold_button.get_active())
		doSurround("<FONT SIZE=\"5\">", "</FONT>");
	else
		doAdvancePast("<FONT SIZE=\"5\">", "</FONT>");
}

void SendMsgDlgTab::on_normalfont_button_toggled()
{
	if (m_bold_button.get_active())
		doSurround("<FONT SIZE=\"3\">", "</FONT>");
	else
		doAdvancePast("<FONT SIZE=\"3\">", "</FONT>");
}

void SendMsgDlgTab::on_smallfont_button_toggled()
{
	if (m_bold_button.get_active())
		doSurround("<FONT SIZE=\"1\">", "</FONT>");
	else
		doAdvancePast("<FONT SIZE=\"1\">", "</FONT>");
}

void SendMsgDlgTab::on_selectfont_button_toggled()
{
}

void SendMsgDlgTab::on_fgcolor_button_toggled()
{
}

void SendMsgDlgTab::on_bgcolor_button_toggled()
{
}

void SendMsgDlgTab::on_smilies_button_toggled()
{
}

void SendMsgDlgTab::on_send_button_clicked()
{
	m_bold_button.set_active(false);
	m_italic_button.set_active(false);
	m_underline_button.set_active(false);
	m_bigfont_button.set_active(false);
	m_normalfont_button.set_active(false);
	m_smallfont_button.set_active(false);
	m_selectfont_button.set_active(false);
	m_fgcolor_button.set_active(false);
	m_bgcolor_button.set_active(false);
	m_smilies_button.set_active(false);
	
	Glib::RefPtr<Gtk::TextBuffer> buffer;
	buffer = m_input_textview.get_buffer();
	if (buffer->size() != 0) {
		
		Glib::ustring text = buffer->get_text(false);
		linqMain->getUDPSession()->sendMessage(MSG_TEXT, name.c_str(), text.c_str());
		m_input_textview.get_buffer()->erase(buffer->begin(), buffer->end());
		
		time_t when;
		when = time(NULL);
		char mdate[64];
		strftime(mdate, sizeof(mdate), "%H:%M:%S", localtime(&when));
		std::string str;
		str = "<FONT SIZE=\"2\">(";
		str += mdate;
		str += ") </FONT><B>";
		str += linqMain->myInfo.name;
		str += ": ";
		str += text;
		str += "</B><BR>";
	
		gtk_imhtml_append_text(GTK_IMHTML(imhtml), str.c_str(), str.length(), (GtkIMHtmlOptions)0);
	}
	m_input_textview.grab_focus();
}

void SendMsgDlgTab::on_warning_button_clicked()
{
}

void SendMsgDlgTab::on_info_button_clicked()
{
	linqMain->main_win->getContactInfo(name.c_str());
}

bool SendMsgDlgTab::on_input_textview_event(GdkEvent *e)
{
    if (e->type != GDK_KEY_PRESS)
    	return false;

    GdkEventKey* event = (GdkEventKey*)e;

	bool return_val=true;  //if return TRUE,the widget which in the main window will not receive any keyboard event.
	
	bool only_ctrl_pressed = ((event->state & GDK_CONTROL_MASK)&&(!(event->state & GDK_MOD1_MASK))&&(!(event->state & GDK_SHIFT_MASK)));
	//gboolean only_mod1_pressed = ((event->state & GDK_MOD1_MASK)&&(!(event->state & GDK_CONTROL_MASK))&&(!(event->state & GDK_SHIFT_MASK)));

	switch (event->keyval) {
		case GDK_Return:
			if (event->type==GDK_KEY_PRESS && only_ctrl_pressed) {
				on_send_button_clicked();
			}
			else
				return_val = false;
			break;
		default:
			return_val = false;
			break;
	}

	return return_val;
}

void SendMsgDlgTab::doSurround(const Glib::ustring &pre, const Glib::ustring &post)
{	
	Glib::RefPtr<Gtk::TextBuffer> entry_buffer;
	entry_buffer = m_input_textview.get_buffer();

	Gtk::TextBuffer::iterator start, end;
	entry_buffer->get_selection_bounds(start, end);
	if (start != end) {
		Gtk::TextBuffer::iterator m_start, m_end;
		while (start.forward_search(pre, Gtk::TEXT_SEARCH_VISIBLE_ONLY, m_start, m_end, end)) {
			entry_buffer->erase(m_start, m_end);
			entry_buffer->get_selection_bounds(start, end);
		}
		while (start.forward_search(post, Gtk::TEXT_SEARCH_VISIBLE_ONLY, m_start, m_end, end)) {
			entry_buffer->erase(m_start, m_end);
			entry_buffer->get_selection_bounds(start, end);
		}

		Glib::RefPtr<Gtk::TextBuffer::Mark> mark_start, mark_end;		
		mark_start = entry_buffer->create_mark("m1", start, true);
		mark_end = entry_buffer->create_mark("m2", end, false);
		entry_buffer->insert(start, pre);
		entry_buffer->get_selection_bounds(start, end);
		entry_buffer->insert(end, post);
		start = entry_buffer->get_iter_at_mark(mark_start);
		entry_buffer->move_mark_by_name("selection_bound", start);
	}
	else {
		entry_buffer->insert(start, pre);
		start = entry_buffer->get_insert()->get_iter();
		entry_buffer->insert(start, post);
		start = entry_buffer->get_insert()->get_iter();
		start.backward_chars(strlen(post.c_str()));
		entry_buffer->place_cursor(start);
	}

	m_input_textview.grab_focus();
}

bool SendMsgDlgTab::invertTags(const char *s1, const char *s2, bool really)
{
	Gtk::TextBuffer::iterator start1, start2, end1, end2;
	Glib::ustring b1, b2;

	Glib::RefPtr<Gtk::TextBuffer> entry_buffer;
	entry_buffer = m_input_textview.get_buffer();

	entry_buffer->get_selection_bounds(start1, end2);
	if (start1 != end2) {
		start2 = start1;
		end1 = end2;

		if (!start2.forward_chars((int)(strlen(s1))))
			return false;

		if (!end1.backward_chars((int)(strlen(s2))))
			return false;

		b1 = entry_buffer->get_text(start1, start2, false);
		b2 = entry_buffer->get_text(end1, end2, false);

		if (!g_ascii_strncasecmp(b1.c_str(), s1, strlen(s1)) &&
		    !g_ascii_strncasecmp(b2.c_str(), s2, strlen(s2))) {

			if (really) {
				Glib::RefPtr<Gtk::TextBuffer::Mark> m_end1, m_end2;
 
				m_end1 = entry_buffer->create_mark("m1", end1, true);
				m_end2 = entry_buffer->create_mark("m2", end2, true);

				entry_buffer->erase(start1, start2);
				end1 = entry_buffer->get_iter_at_mark(m_end1);
				end2 = entry_buffer->get_iter_at_mark(m_end2);
				entry_buffer->erase(end1, end2);
				entry_buffer->delete_mark(m_end1);
				entry_buffer->delete_mark(m_end2);
			}
			return true;
		}
	}

	return false;
}

void SendMsgDlgTab::doAdvancePast(const Glib::ustring &pre, const Glib::ustring &post)
{
	if (invertTags(pre.c_str(), post.c_str(), true))
		return;

	Gtk::TextBuffer::iterator current_pos, start, end;

	Glib::RefPtr<Gtk::TextBuffer> entry_buffer;
	entry_buffer = m_input_textview.get_buffer();

	current_pos = entry_buffer->get_iter_at_mark(entry_buffer->get_insert());

	if (current_pos.forward_search(post, Gtk::TEXT_SEARCH_VISIBLE_ONLY, start, end, entry_buffer->end()))
		entry_buffer->place_cursor(end);
	else
		entry_buffer->insert_at_cursor(post);

	m_input_textview.grab_focus();
}
