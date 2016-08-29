#ifdef linux
	#include <libappindicator/app-indicator.h>
	#include <gtk/gtk.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/keysym.h>
	#include <X11/keysymdef.h>
	#include <gdk/gdkkeysyms.h>

	#include <unistd.h>
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
	AppIndicator *indicator;

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
		gchar *key_name;
        int i;

        /* Callback methods */
        _parse_callback = parse_callback;
        _exit_callback = exit_callback;

        /* Init equation stores */
        for (i=0; i<MAX_EQUATIONS; ++i)
			stored_entries[i] = NULL;

		/* Start up */
		gtk_init(0, NULL);
  		keybinder_init();

  		/* Default key code */
		if (get_setting(SETTING_HOTMOD) == 0 || get_setting(SETTING_HOTKEY) == 0) {
			set_setting(SETTING_HOTMOD, GDK_CONTROL_MASK);
			set_setting(SETTING_HOTKEY, XK_space);
		}

		/* Set hotkey */
		key_name = gtk_accelerator_name(get_setting(SETTING_HOTKEY), get_setting(SETTING_HOTMOD));
		keybinder_bind(key_name, handler, NULL);
		g_free(key_name);

    	app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
    	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    	g_application_register ((GApplication*) app, NULL, NULL);
    	g_application_activate ((GApplication*) app);
  		g_object_unref ((GApplication*) app);

		/* Indicator */
		indicator = app_indicator_new ("lavendeux-indicator", "lavendeux", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
		app_indicator_set_status (indicator, APP_INDICATOR_STATUS_ACTIVE);
		app_indicator_set_attention_icon (indicator, "lavendeux");
		app_indicator_set_title(indicator, "Lavendeux");
		app_indicator_set_menu (indicator, GTK_MENU (get_menu()));
    }

    static void activate (GtkApplication* app, gpointer user_data) { }

    static void handler (const char *keystring, void *user_data) {
    	if (get_setting(SETTING_AUTOCOPY) == SETTING_AUTOCOPY_ON) 
    		auto_copy();

    	const wchar_t* str = from_clipboard();
    	if (!str) return;

    	_parse_callback( str );

    	if (get_setting(SETTING_AUTOCOPY) == SETTING_AUTOCOPY_ON)
    		auto_paste();
    }

    static void copy_equation (GtkWidget* app, gpointer user_data) {
    	to_clipboard((wchar_t*) user_data);
    }

	XKeyEvent createKeyEvent(Display *display, Window win, Window winRoot, int press, int keycode, int modifiers) {
		XKeyEvent event;

		event.display     = display;
		event.window      = win;
		event.root        = winRoot;
		event.subwindow   = None;
		event.time        = CurrentTime;
		event.x           = 1;
		event.y           = 1;
		event.x_root      = 1;
		event.y_root      = 1;
		event.same_screen = True;
		event.keycode     = XKeysymToKeycode(display, keycode);
		event.state       = modifiers;

		if(press)
			event.type = KeyPress;
		else
			event.type = KeyRelease;

		return event;
	}

	void auto_key(int keycode, int modifiers) {
		XKeyEvent event;
		Window winFocus;
		int revert;

		/* Get X11 display */
		Display *display = XOpenDisplay(0);
		if (!display) return;

		/* Get root window, and focused window */
		Window winRoot = XDefaultRootWindow(display);
		XGetInputFocus(display, &winFocus, &revert);

		/* Key release */
		event = createKeyEvent(display, winFocus, winRoot, 0, keycode, modifiers);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

		/* Keypress */
		event = createKeyEvent(display, winFocus, winRoot, 1, keycode, modifiers);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

		/* Key release */
		event = createKeyEvent(display, winFocus, winRoot, 0, keycode, modifiers);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
		
		XFlush(display);
		XCloseDisplay(display);
		g_usleep(100000);
	}

    void key_registrar( void ) {
		GtkWidget *label, *window;
		gchar *key_current;
		char text[0xFF];

		/* Create window */
		window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_container_set_border_width(GTK_CONTAINER (window), 10);
		gtk_window_set_title(GTK_WINDOW(window), "Hotkey selection");

		/* Set up label */
		key_current = gtk_accelerator_get_label(get_setting(SETTING_HOTKEY), get_setting(SETTING_HOTMOD));
		sprintf(text, "Current hotkey is %s\nPlease enter the new hotkey", key_current);
		label = gtk_label_new(text);
		gtk_container_add (GTK_CONTAINER (window), label);

		/* Signal connect */
		gtk_widget_show_all (window);
		g_signal_connect(GTK_WINDOW(window), "key-press-event", G_CALLBACK(registrar_keystroke), label);

		/* Clean up */
		g_free(key_current);
	}

	static void registrar_keystroke(GtkWidget *widget, GdkEventKey *event, GtkWidget *label) {
		guint modifiers, keyval;
		gchar *key_name, *key_label, *key_current;
		char text[0xFF];

		/* Get values from event */
		modifiers = event->state & (GDK_SHIFT_MASK | GDK_CONTROL_MASK | GDK_MOD1_MASK);
		keyval = event->keyval;

		/* String representations */
		key_current = gtk_accelerator_name(get_setting(SETTING_HOTKEY), get_setting(SETTING_HOTMOD));
		key_name = gtk_accelerator_name(keyval, modifiers);
		key_label = gtk_accelerator_get_label(keyval, modifiers);

		/* Validate */
		if (modifiers == 0 || keyval == 0)
			return;

		/* Bind new hotkey */
		keybinder_unbind(key_current, handler);
		keybinder_bind(key_name, handler, NULL);

		/* Label */
		sprintf(text, "Hotkey set to %s", key_label);
		gtk_label_set_text(GTK_LABEL (label), text);

		/* Settings */
		set_setting(SETTING_HOTMOD, modifiers);
		set_setting(SETTING_HOTKEY, keyval);

		/* Clean up */
		g_free(key_current);
		g_free(key_name);
		g_free(key_label);
	}

    void debug_enable( void ) {}

	static void close_app (GtkWidget *widget, gpointer data) {
		gchar *key_current;

		key_current = gtk_accelerator_name(get_setting(SETTING_HOTKEY), get_setting(SETTING_HOTMOD));
		keybinder_unbind(key_current, handler);

		g_free(key_current);
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
		g_signal_connect (item, "toggled", G_CALLBACK (toggle_silent_mode), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_SILENT)==SETTING_SILENT_ON));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
		
		/* Auto copy/paste mode */
		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_ENABLEAUTOCOPY));
		g_signal_connect (item, "toggled", G_CALLBACK (toggle_auto_copypaste), NULL);
		gtk_check_menu_item_set_active((GtkCheckMenuItem*) item, (get_setting(SETTING_AUTOCOPY)==SETTING_AUTOCOPY_ON));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

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

		/* Regkey */
		item = gtk_menu_item_new_with_label(language_char_str(LANG_STR_HOTKEY));
		g_signal_connect (item, "activate", G_CALLBACK (key_registrar), NULL);
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
		set_setting(SETTING_SILENT, gtk_check_menu_item_get_active((GtkCheckMenuItem*) widget) ? SETTING_SILENT_ON : SETTING_SILENT_OFF );
	}

	static void toggle_auto_copypaste (GtkWidget *widget, gpointer data) {
		set_setting(SETTING_AUTOCOPY, gtk_check_menu_item_get_active((GtkCheckMenuItem*) widget) ? SETTING_AUTOCOPY_ON : SETTING_AUTOCOPY_OFF);
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

        /* Update indicator menu */
        app_indicator_set_menu (indicator, GTK_MENU (get_menu()));
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
        FILE* test;
        char *path, *self;
        
        /* Prepare to hold path */
        path = (char*) malloc(sizeof(char)*(MAX_PATH+1));
        if (path == NULL)
            error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);

        /* Preferences first */
        if (strlen(prefered_path) != 0) {
            strcpy(path, prefered_path);
            return path;
        }

        /* Test current directory */
        test = fopen(CONFIG_FILENAME, "r");
        if (test != NULL) {
            fclose(test);

            path[0] = '\0';
            strcat(path, CONFIG_FILENAME);

            printf("Found configuration path: %s\n", path);
            return path;
        }

        strcpy(path, "~/");
        strcat(path, CONFIG_FILENAME);
        printf("Found configuration path: %s\n", path);
        return path;
	}

	char* self_path( void ) {
        char *path;

        /* Prepare to hold path */
        path = (char*) malloc(sizeof(char)*(MAX_PATH+1));
        if (path == NULL)
            error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);

        strcpy(path, ETC_PATH);
        return path;
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

		if (get_setting(SETTING_SILENT) == SETTING_SILENT_ON)
			return;

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
