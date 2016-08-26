#ifdef linux
	#include <libappindicator/app-indicator.h>
	#include <gtk/gtk.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/keysym.h>
	#include <X11/keysymdef.h>
	#include <gdk/gdkkeysyms.h>

	#include <pthread.h>
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <wchar.h>
	#include <keybinder-3.0/keybinder.h>

	#define ETC_PATH "/etc/lavendeux/"
	#define HOT_KEY "<Ctrl>space"

	/* Menu equations */
	wchar_t *stored_entries[MAX_EQUATIONS];

	#define MAX_PATH 2048
    char prefered_path[MAX_PATH+1];

	/* Auto copy and paste */
	#define auto_copy() auto_key(XK_C, GDK_CONTROL_MASK)
	#define auto_paste() auto_key(XK_V, GDK_CONTROL_MASK)

	#define APPLICATION_ID "richardcarson.lavendeux"

	static void close_app (GtkWidget*, gpointer);
	static void activate (GtkApplication*, gpointer);
	static void handler (const char*, void*);
	static void registrar_keystroke(GtkWidget*, GdkEventKey*, GtkWidget*);

	static GtkWidget* get_menu ();
	static void copy_equation (GtkWidget*, gpointer);

	/* Settings */
	static void toggle_silent_mode (GtkWidget*, gpointer);
	static void toggle_auto_copypaste (GtkWidget*, gpointer);

	/* Languages */
	static void setlang_en (GtkWidget*, gpointer);
	static void setlang_fr (GtkWidget*, gpointer);

	/* Angles */
	static void setangle_deg (GtkWidget*, gpointer);
	static void setangle_rad (GtkWidget*, gpointer);

	/* Event creation */
	XKeyEvent createKeyEvent(Display*, Window, Window, int, int, int);
	void auto_key(int, int);
#endif