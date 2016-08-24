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

	#include "language.h"
	#include "settings.h"
	#include "lavendeux.h"
	#include "interface.h"
	#include "interface_linux.h"
	#include "cmdflags.h"
	
	/* Event globals */
	GtkWidget *window;

	/* Stored callbacks */
	parseCallback _parse_callback;
	exitCallback _exit_callback;

	/** 
	 * Prepare and draw the interface 
     * @param exit_callback Method to be called in event of a quit
     * @param parse_callback Method to be called in event of the hotkeys being pressed
     */
    void init_interface(exitCallback exit_callback, parseCallback parse_callback) {
    	GtkApplication *app;
		GtkWidget* widget;
		GtkWidget* button_box;
		AppIndicator *indicator;
        int i;

        /* Callback methods */
        _parse_callback = parse_callback;
        _exit_callback = exit_callback;

        /* Init equation stores */
        for (i=0; i<MAX_EQUATIONS; ++i)
			stored_entries[i] = NULL;

		gtk_init(0, NULL);

  		keybinder_init();
  		keybinder_bind(HOT_KEY, handler, NULL);	

    	app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
    	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    	g_application_register ((GApplication*) app, NULL, NULL);
    	g_application_activate ((GApplication*) app);
  		g_object_unref ((GApplication*) app);

		/* Indicator */
		indicator = app_indicator_new ("lavendeux-indicator", "lavendeux", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
		app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
		app_indicator_set_attention_icon (indicator, "lavendeux");
		app_indicator_set_title(indicator, "Lavendeux")
		app_indicator_set_menu (indicator, GTK_MENU (get_menu()));
    }

    static void handler (const char *keystring, void *user_data) {
    	const wchar_t* str = from_clipboard();
    	if (!str) return;

    	if (0 && get_setting(SETTING_AUTOCOPY) == SETTING_AUTOCOPY_ON)
    		auto_copy();

    	_parse_callback( str );

    	if (0 && get_setting(SETTING_AUTOCOPY) == SETTING_AUTOCOPY_ON)
    		auto_paste();
    }

    static void copy_equation (GtkWidget* app, gpointer user_data) {
    	to_clipboard((wchar_t*) user_data);
    	_parse_callback( (wchar_t*) user_data );
    }

    /* Not working! */
    void auto_copy( void ) {
    	GdkKeymapKey *keys;
    	gint n_keys;

    	if (gdk_keymap_get_entries_for_keyval (gdk_keymap_get_default (), GDK_KEY_C, &keys, &n_keys)) {
      		guint16 hardware_keycode;
      		GdkEvent *event;

      		hardware_keycode = keys[0].keycode;
      		g_free(keys);

			event = gdk_event_new (GDK_KEY_PRESS);
			event->key.window = gdk_screen_get_active_window(gdk_screen_get_default());
			event->key.state = GDK_CONTROL_MASK;
			event->key.hardware_keycode = hardware_keycode;

			event->key.keyval = gdk_unicode_to_keyval (GDK_KEY_C);
			event->key.length = 1;
			event->key.string = g_strdup ("c");

			event->key.send_event = FALSE;
			event->key.time = GDK_CURRENT_TIME;

			gtk_main_do_event (event);
			gdk_event_free (event);
		}
    }

    /* Not working! */
    void auto_paste( void ) {
    	GdkKeymapKey *keys;
    	gint n_keys;

    	if (gdk_keymap_get_entries_for_keyval (gdk_keymap_get_default (), GDK_KEY_V, &keys, &n_keys)) {
      		guint16 hardware_keycode;
      		GdkEvent *event;

      		hardware_keycode = keys[0].keycode;
      		g_free(keys);

			event = gdk_event_new (GDK_KEY_PRESS);
			event->key.window = gdk_screen_get_active_window(gdk_screen_get_default());
			event->key.state = GDK_CONTROL_MASK;
			event->key.hardware_keycode = hardware_keycode;

			event->key.keyval = gdk_unicode_to_keyval (GDK_KEY_V);
			event->key.length = 1;
			event->key.string = g_strdup ("v");

			event->key.send_event = FALSE;
			event->key.time = GDK_CURRENT_TIME;

			gtk_main_do_event (event);
			gdk_event_free (event);
		}
    }

    /* Not implemented yet */
    void key_registrar( void ) {}

    void debug_enable( void ) {}

	static void activate (GtkApplication* app, gpointer user_data) {
		char* help = cmdflag_help(HELP_MSG);
		GtkWidget* widget;
		GtkWidget* button_box;
		GtkWidget *view;
		GtkTextBuffer *buffer;
		GtkWidget *container;
	}

	static void close_app (GtkWidget *widget, gpointer data) {
		keybinder_unbind(HOT_KEY, handler);
		_exit_callback();
	}

	static GtkWidget* get_menu () {
		int i;
		GtkWidget *menu, *submenu, *item;
		menu = gtk_menu_new();
		submenu = gtk_menu_new();

		/* equations */
		for (i=0; i<MAX_EQUATIONS; ++i)
            if (stored_entries[i] != NULL) {
		    	char* str = (char*) malloc(sizeof(wchar_t)*(wcslen(stored_entries[i])+1));
		    	str[ wcstombs(str, stored_entries[i], wcslen(stored_entries[i])) ] = '\0';

				item = gtk_menu_item_new_with_label(str);
				g_signal_connect (item, "activate", G_CALLBACK (copy_equation), stored_entries[i]);
				gtk_widget_show(item);
				gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
			}

		/* Seperator */
		item = gtk_separator_menu_item_new();
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		item = gtk_check_menu_item_new_with_label("English");
		g_signal_connect (item, "select", G_CALLBACK (setlang_en), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_LANG) == LANG_EN));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		item = gtk_check_menu_item_new_with_label("Francais");
		g_signal_connect (item, "select", G_CALLBACK (setlang_fr), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_LANG) == LANG_FR));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		/* Languages menu */
		item = gtk_menu_item_new_with_label(language_char_str(LANG_STR_LANGUAGE));
		gtk_menu_item_set_submenu((GtkMenuItem*) item, submenu);
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		/* Silent mode */
		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_SILENT_ERRS));
		g_signal_connect (item, "select", G_CALLBACK (toggle_silent_mode), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_SILENT)==SETTING_SILENT_ON)); //
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
		
		/* Auto copy/paste mode -- NOT WORKING
		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_ENABLEAUTOCOPY));
		g_signal_connect (item, "select", G_CALLBACK (toggle_auto_copypaste), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_AUTOCOPY)==SETTING_AUTOCOPY_ON));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item); */

		submenu = gtk_menu_new();

		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_DEGREES));
		g_signal_connect (item, "select", G_CALLBACK (setangle_deg), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_RADIANS));
		g_signal_connect (item, "select", G_CALLBACK (setangle_rad), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_ANGLE) == SETTING_ANGLE_RAD));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		/* Angles menu */
		item = gtk_menu_item_new_with_label(language_char_str(LANG_STR_ANGLE_UNITS));
		gtk_menu_item_set_submenu((GtkMenuItem*) item, submenu);
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		/* Seperator */
		item = gtk_separator_menu_item_new();
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		/* About */
		item = gtk_menu_item_new_with_label(language_char_str(LANG_STR_ABOUT));
		g_signal_connect (item, "activate", G_CALLBACK (print_help), NULL);
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		/* Exit */

		item = gtk_menu_item_new_with_label(language_char_str(LANG_STR_EXIT));
		g_signal_connect (item, "activate", G_CALLBACK (close_app), NULL);
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		return menu;
	}

	static void toggle_silent_mode (GtkWidget *widget, gpointer data) {
		set_setting(SETTING_SILENT, get_setting(SETTING_SILENT)==SETTING_SILENT_ON ? SETTING_SILENT_OFF : SETTING_SILENT_ON);
	}

	static void toggle_auto_copypaste (GtkWidget *widget, gpointer data) {
		set_setting(SETTING_AUTOCOPY, get_setting(SETTING_AUTOCOPY)==SETTING_AUTOCOPY_ON ? SETTING_AUTOCOPY_OFF : SETTING_AUTOCOPY_ON);
	}

	static void setlang_en (GtkWidget *widget, gpointer data) {
		set_setting(SETTING_LANG, LANG_EN);
		language_set_current(LANG_EN);
	}

	static void setlang_fr (GtkWidget *widget, gpointer data) {
		set_setting(SETTING_LANG, LANG_FR);
		language_set_current(LANG_FR);
	}

	static void setangle_deg (GtkWidget *widget, gpointer data) {
		set_setting(SETTING_ANGLE, SETTING_ANGLE_DEG);
	}

	static void setangle_rad (GtkWidget *widget, gpointer data) {
		set_setting(SETTING_ANGLE, SETTING_ANGLE_RAD);
	}
	
	/** 
	 * Check window messages, process events
	 */
	void update_interface( void ) {
		while (gtk_events_pending ()) {
			g_usleep (1);
			gtk_main_iteration ();
		}
		g_usleep (10000);
	}
	
	/** 
	 * Print help message to stdout
	 */
	void print_help( void ){
		char* help = cmdflag_help(HELP_MSG);
		GtkWidget *content_area;
		GtkWidget *label;
		GtkWidget *dialog;
		GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

		dialog = gtk_message_dialog_new ((GtkWindow*) window,
			flags,
			GTK_MESSAGE_INFO,
			GTK_BUTTONS_OK,
			"%s",
			help);

		gtk_widget_set_size_request(dialog, 400, 300);

		g_signal_connect_swapped (dialog,
			"response",
			G_CALLBACK (gtk_widget_destroy),
			dialog);

		gtk_dialog_run (GTK_DIALOG(dialog));
	}
	
	/** 
	 * Add an entry to the menu history
	 * @param entry The string to add
	 */
	void add_history(const wchar_t *entry) {
        int i;

        /* Free memory for last entry */
        if (stored_entries[MAX_EQUATIONS-1] != NULL)
            free(stored_entries[MAX_EQUATIONS-1]);

        /* Shift all current entries by one */
        for (i=MAX_EQUATIONS-2; i>=0; --i)
            stored_entries[i+1] = stored_entries[i];

        /* Allocate new entry */
        stored_entries[0] = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen(entry)+1));
        if (stored_entries[0] == NULL)
            error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);
        wcscpy( stored_entries[0], entry);
    }

	/** 
	 * Copy string to system clipboard
	 * @param entry The string to add
	 */
	void to_clipboard(const wchar_t *entry) {
		GtkClipboard *clipboard = gtk_clipboard_get_default( gdk_display_get_default() );
		char* text = malloc(sizeof(wchar_t)*(wcslen(entry)+1));
		if (text == NULL) return;


		text[ wcstombs(text, entry, wcslen(entry)) ] = '\0';
		gtk_clipboard_set_text(clipboard, text, strlen(text));
	}

	/** 
	 * Get string from system clipboard
	 *
	 * @return const wchar_t* The string fetched
	 */
	const wchar_t* from_clipboard( void ){
		wchar_t* wtext;
		const char* text;
		GtkClipboard *clipboard = gtk_clipboard_get_default( gdk_display_get_default() );

		text = gtk_clipboard_wait_for_text(clipboard);
		if (text == NULL) return NULL;

		wtext = malloc(sizeof(wchar_t)*(strlen(text)+1));
		if (wtext == NULL) {
			g_free((gpointer) text);
			return NULL;
		}

		wtext[ mbstowcs(wtext, text, strlen(text)) ] = L'\0';

		g_free((gpointer) text);
		return wtext;
	}

	/**
	 * Get the path to a valid configuration file
	 * Search in current directory, then relevant home dir
	 *
	 * @return Path to the configuration file
	 */
	char* config_path( void ) {
		char* path = malloc(sizeof(char) * 6);
		strcpy(path, "12345");
		return path;
	}

	char* self_path( void ) {
		return "./";
	}

	void config_set(const char* path) {
		strcpy(prefered_path, path);
	}

	/**
	 * Get the path to a valid configuration file
	 * @param title The title of the error message
	 * @param msg The error message
	 * @param fatal if non 0, exit
	 */
	void error_msg(const wchar_t* title, const wchar_t* msg, char fatal) {
		char* help = cmdflag_help(HELP_MSG);
		GtkWidget *content_area;
		GtkWidget *label;
		GtkWidget *dialog;
		GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;

		dialog = gtk_message_dialog_new ((GtkWindow*) window,
			flags,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_OK,
			"%S",
			msg);

		gtk_widget_set_size_request(dialog, 400, 300);

		g_signal_connect_swapped (dialog,
			"response",
			(fatal) ? G_CALLBACK(close_app) : G_CALLBACK (gtk_widget_destroy),
			dialog);

		gtk_dialog_run (GTK_DIALOG(dialog));
	}
#endif
