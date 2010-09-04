#ifndef __LINQ_SKIN_H__
#define __LINQ_SKIN_H__

#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

struct SkinMainMenu
{
	Glib::RefPtr<Gdk::Pixbuf> icon;
	Glib::RefPtr<Gdk::Pixbuf> netbookmark,gourpfriend,msghistory,memorandum,mobilemsg,
			  changeskin,changeuser,regwizard,myoption,sysoption,help,quit;
};

struct SkinBuddyMenu
{
	Glib::RefPtr<Gdk::Pixbuf> send_message,send_mail,chat_log,view_info;
};

struct SkinLinq
{
	Glib::RefPtr<Gdk::Pixbuf> icon;
	Glib::RefPtr<Gdk::Pixbuf> face[85];
	
	Glib::RefPtr<Gdk::Pixbuf> buddy;
	Glib::RefPtr<Gdk::Pixbuf> service;
	Glib::RefPtr<Gdk::Pixbuf> mobile;
	Glib::RefPtr<Gdk::Pixbuf> chain;
	Glib::RefPtr<Gdk::Pixbuf> chatroom;
	Glib::RefPtr<Gdk::Pixbuf> search;
	Glib::RefPtr<Gdk::Pixbuf> mozilla;
	Glib::RefPtr<Gdk::Pixbuf> sysmsg;
	SkinMainMenu main_menu;	
	SkinBuddyMenu buddy_menu;
	Glib::RefPtr<Gdk::Pixbuf> status[4];
};

class Skin
{
public:
	Skin();	
	SkinLinq linq;
};

#endif
