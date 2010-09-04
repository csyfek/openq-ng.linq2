## Process this file with automake to produce Makefile.in
SUBDIRS= face pixmaps sound win32

bin_PROGRAMS = linq

INCLUDES =							\
	$(LINQ_CFLAGS)						\
	-DLINQ_LOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale"\"		\
	-DDATA_DIR=\""$(datadir)"\"		\
	-DG_DISABLE_DEPRECATED					\
	-DGDK_DISABLE_DEPRECATED				\
	-DGTK_DISABLE_DEPRECATED				\
	-DGDK_PIXBUF_DISABLE_DEPRECATED				\
	-DGNOME_DISABLE_DEPRECATED


linq_LDFLAGS = 

linq_LDADD = 				\
	/usr/lib/liblinq.so	\
	$(LINQ_LIBS)			

linq_SOURCES = 			\
	LinQ.cpp LinQ.h			\
	LinqMain.cpp LinqMain.h	\
	skin.cpp skin.h		\
	intl.h				\
	gtksocketregistry.cpp gtksocketregistry.h	\
	GtkSocket.cpp GtkSocket.h	\
	LinqWindow.cpp LinqWindow.h	\
	LinqDB.cpp LinqDB.h	\
	LinqContactInfo.cpp LinqContactInfo.h \
	LinqMsg.cpp LinqMsg.h \
	LinqOption.cpp LinqOption.h \
	LinqUserList.cpp LinqUserList.h \
	LinqGlobalSetting.cpp LinqGlobalSetting.h \
	LoginDlg.cpp LoginDlg.h	\
	MainWin.cpp MainWin.h	\
	MainWin_ContactView.cpp MainWin_ContactView.h	\
	MainWin_ServiceView.cpp MainWin_ServiceView.h	\
	MainWin_MobileView.cpp MainWin_MobileView.h	\
	MainWin_ChainView.cpp MainWin_ChainView.h	\
	UserInfoDlg.cpp UserInfoDlg.h	\
	PrefsDlg.cpp PrefsDlg.h	\
	gtkimhtml.c gtkimhtml.h			\
	SendMsgDlg.cpp SendMsgDlg.h		\
	sha.cpp sha.h	\
	SysMsgDlg.cpp SysMsgDlg.h	\
	SysMsgListDlg.cpp SysMsgListDlg.h	\
	MsgManager.cpp MsgManager.h \
	SearchDlg.cpp SearchDlg.h	\
	SendFileDlg.cpp SendFileDlg.h	\
	RegWizard.cpp RegWizard.h

EXTRA_DIST = Makefile.mingw
