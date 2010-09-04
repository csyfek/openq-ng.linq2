SUBDIRS = intl m4 po src

desktopdir = $(datadir)/applications
desktop_DATA = linq.desktop
pixmapdir = $(datadir)/pixmaps
pixmap_DATA = linq.png

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = autogen.sh linq.desktop linq.png linq.spec \
			 README.win32 Makefile.mingw po/Makefile.mingw config.h.mingw linq-installer.nsi linq.ico linq-uninst.ico\
			 config.rpath mkinstalldirs intltool-extract.in intltool-merge.in intltool-update.in
