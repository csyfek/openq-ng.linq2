#include "config.h"
#include "LinQ.h"
#include "skin.h"
#include "stdio.h"
#include <linq/sessionlistener.h>

Skin::Skin()
{
	std::string filename;
	
#ifdef __WIN32
	filename = Glib::build_filename(linqDataDir, "pixmaps/linq.png");
	linq.icon = Gdk::Pixbuf::create_from_file(filename);
#else
	linq.icon = Gdk::Pixbuf::create_from_file(DATA_DIR "/pixmaps/linq.png");
#endif
	gchar tmpstr[256];
	for (int i=0;i<85;i++) {		
		sprintf(tmpstr, "face/%d.png",i+1);
		filename = Glib::build_filename(linqDataDir, tmpstr);
		linq.face[i] = Gdk::Pixbuf::create_from_file(filename);
	}

	filename = Glib::build_filename(linqDataDir, "pixmaps/buddy.png");
	linq.buddy = Gdk::Pixbuf::create_from_file(filename);	
	filename = Glib::build_filename(linqDataDir, "pixmaps/service.png");
	linq.service = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/mobile.png");
	linq.mobile = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/chain.png");
	linq.chain = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/chatroom.png");
	linq.chatroom = Gdk::Pixbuf::create_from_file(filename);	
	filename = Glib::build_filename(linqDataDir, "pixmaps/search.png");
	linq.search = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/mozilla.png");
	linq.mozilla = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/sysmsg.png");
	linq.sysmsg = Gdk::Pixbuf::create_from_file(filename);

	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_icon.png");
	linq.main_menu.icon = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_netbookmark.png");
	linq.main_menu.netbookmark = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_groupfriend.png");
	linq.main_menu.gourpfriend = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_msghistory.png");
	linq.main_menu.msghistory = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_memorandum.png");
	linq.main_menu.memorandum = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_mobilemsg.png");
	linq.main_menu.mobilemsg = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_changeskin.png");
	linq.main_menu.changeskin = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_changeuser.png");
	linq.main_menu.changeuser = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_regwizard.png");
	linq.main_menu.regwizard = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_myoption.png");
	linq.main_menu.myoption = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_sysoption.png");
	linq.main_menu.sysoption = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_help.png");
	linq.main_menu.help = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/menu_quit.png");
	linq.main_menu.quit = Gdk::Pixbuf::create_from_file(filename);
	
	filename = Glib::build_filename(linqDataDir, "pixmaps/buddy_menu_sendmsg.png");
	linq.buddy_menu.send_message = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/buddy_menu_sendmail.png");
	linq.buddy_menu.send_mail = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/buddy_menu_chatlog.png");
	linq.buddy_menu.chat_log = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/buddy_menu_viewinfo.png");
	linq.buddy_menu.view_info = Gdk::Pixbuf::create_from_file(filename);

	filename = Glib::build_filename(linqDataDir, "pixmaps/status_offline.png");
	linq.status[STATUS_OFFLINE] = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/status_online.png");
	linq.status[STATUS_ONLINE] = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/status_away.png");
	linq.status[STATUS_AWAY] = Gdk::Pixbuf::create_from_file(filename);
	filename = Glib::build_filename(linqDataDir, "pixmaps/status_invis.png");
	linq.status[STATUS_INVIS] = Gdk::Pixbuf::create_from_file(filename);
}
