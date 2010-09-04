/* LinQ - LinQ Instant Messaging Client.
 * http://myicq.cosoft.org.cn
 * Copyright (C) 2003-2003 HuZheng <huzheng_001@163.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the LinQ Team and others 2003-2003.  See the AUTHORS
 * file for a list of people on the LinQ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * LinQ at http://myicq.cosoft.org.cn.
 */

#include "config.h"
#include "LinQ.h"
#include "gtksocketregistry.h"
#include "LinqMain.h"
#include "intl.h"

#include <gtkmm/main.h>

std::string linqDataDir;

static gboolean setLinqDataDir(void) {
#ifdef __WIN32
	/* Determine ReciteWord Paths during Runtime */
	HMODULE hmod;

	hmod = GetModuleHandle(NULL);
	if( hmod == 0 ) {
		return false;
	}
	char linq_data_dir[256];
	if(GetModuleFileName( hmod, (char*)&linq_data_dir, 256 ) == 0) {
		return false;
	}
	linqDataDir = Glib::path_get_dirname(linq_data_dir);
	return true;
#else
	linqDataDir = DATA_DIR "/linq";
	return true;
#endif
}

#ifdef __WIN32
static void linq_dummy_print( const gchar* string ) {
	return;
}

static void linq_dummy_log_handler (const gchar    *domain,
				    GLogLevelFlags  flags,
				    const gchar    *msg,
				    gpointer        user_data) {
	return;
}
#endif

int main(int argc,char **argv)
{
	if (!setLinqDataDir())
		return 0;

#ifdef __WIN32
	gchar *locale_dir;
	locale_dir = g_strdup_printf("%s" G_DIR_SEPARATOR_S "locale", linqDataDir.c_str());
	bindtextdomain (GETTEXT_PACKAGE, locale_dir);
	g_free(locale_dir);
#else
	bindtextdomain (GETTEXT_PACKAGE, LINQ_LOCALEDIR);
#endif	

	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);

	Gtk::Main kit(argc, argv, true);

#ifdef __WIN32
	/* We don't want a console window.. */
	/*
	 *  Any calls to the glib logging functions, result in a call to AllocConsole().
	 *  ME and 98 will in such cases produce a console window (2000 not), despite
	 *  being built as a windows app rather than a console app.  So we should either
	 *  ignore messages by setting dummy log handlers, or redirect messages.
	 *  This requires setting handlers for all domains (any lib which uses g_logging).
	 */

	g_log_set_handler (NULL, (enum GLogLevelFlags)(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
			   linq_dummy_log_handler, NULL);	
	g_log_set_handler ("Gdk", (enum GLogLevelFlags)(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
			   linq_dummy_log_handler, NULL);
	g_log_set_handler ("Gtk", (enum GLogLevelFlags)(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
			   linq_dummy_log_handler, NULL);
	g_log_set_handler ("GLib", (enum GLogLevelFlags)(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
			   linq_dummy_log_handler, NULL);
	g_log_set_handler ("GModule", (enum GLogLevelFlags)(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
			   linq_dummy_log_handler, NULL);
	g_log_set_handler ("GLib-GObject", (enum GLogLevelFlags)(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
			   linq_dummy_log_handler, NULL);
	g_log_set_handler ("GThread", (enum GLogLevelFlags)(G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL | G_LOG_FLAG_RECURSION),
			   linq_dummy_log_handler, NULL);

	/* g_print also makes a call to AllocConsole(), therefore a handler needs to be
	   set here aswell */
	g_set_print_handler( linq_dummy_print );
#endif

	GtkSocketRegistry *reg;
	reg = new GtkSocketRegistry;
	LinqMain *linq_main;
	linq_main = new LinqMain(reg);
	linq_main->doLogin();
	
	delete linq_main;
	delete reg;
	
	return 0;	
}
