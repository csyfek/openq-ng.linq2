#ifndef __LINQ_WINDOW_H__
#define __LINQ_WINDOW_H__

#include <string>

#include <linq/icqtypes.h>

enum {
	WIN_SEND_MESSAGE,
	WIN_SYS_MESSAGE,
	WIN_SYS_MESSAGE_LIST,
	WIN_VIEW_DETAIL,
	WIN_REG_WIZARD,
	WIN_SEARCH,
	WIN_LOGIN,
	WIN_PREFS,
	WIN_MSG_MANAGER,
};

class LinqWindow {
public:
	LinqWindow(int type);
	virtual ~LinqWindow();

	int type;
};


#endif
