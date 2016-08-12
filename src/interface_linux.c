#ifdef linux
	#include <gtk/gtk.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/keysym.h>
	#include <X11/keysymdef.h>

    #include <pthread.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <wchar.h>

    #include "language.h"
    #include "lavendeux.h"
    #include "interface.h"
	
    /* Event globals */
	GtkWidget *window;

    /* Stored callbacks */
    parseCallback _parse_callback;
    exitCallback _exit_callback;

    /* Menu equations */
    wchar_t *stored_entries[MAX_EQUATIONS];

    /* Options */
    int settings[N_SETTINGS];

    #define APPLICATION_ID "richardcarson.lavendeux"
    static void close_menu (GtkMenuShell*, gpointer);
    static void close_app (GtkWidget*, gpointer);
    static void activate (GtkApplication*, gpointer);
    static void open_menu (GtkWidget*, gpointer);
    void hotkey_wait( void );

    /* Settings */
    static void toggle_silent_mode (GtkWidget*, gpointer);
    static void toggle_auto_copypaste (GtkWidget*, gpointer);

    /* Languages */
    static void setlang_en (GtkWidget*, gpointer);
    static void setlang_fr (GtkWidget*, gpointer);

    /* Angles */
    static void setangle_deg (GtkWidget*, gpointer);
    static void setangle_rad (GtkWidget*, gpointer);


	/** 
     * Prepare and draw the interface 
     * @param exit_callback Method to be called in event of a quit
     * @param parse_callback Method to be called in event of the hotkeys being pressed
     */
    void init_interface(exitCallback exit_callback, parseCallback parse_callback) {
    	pthread_t hotkey_thread;
    	GtkApplication *app;
		GtkWidget* widget;
		GtkWidget* button_box;
        int i;

        /* Callback methods */
        _parse_callback = parse_callback;
        _exit_callback = exit_callback;

        /* Init equation stores */
        for (i=0; i<MAX_EQUATIONS; ++i)
			stored_entries[i] = NULL;

        /* Default options */
        for (i=0; i<N_SETTINGS; ++i)
			settings[i] = 0;

		gtk_init(0, NULL);
    	app = gtk_application_new (APPLICATION_ID, G_APPLICATION_FLAGS_NONE);
    	g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

    	g_application_register (app, NULL, NULL);
    	g_application_activate (app);
  		g_object_unref (app);

  		pthread_create(&hotkey_thread, NULL, hotkey_wait, NULL);
    }

    static void activate (GtkApplication* app, gpointer user_data) {
		GtkWidget* widget;
		GtkWidget* button_box;

		/* Create window */
		window = gtk_application_window_new (app);
		g_signal_connect (window, "delete_event", G_CALLBACK (close_app), NULL);
		gtk_window_set_title (GTK_WINDOW (window), APPLICATION_NAME);

		/* Container */
		button_box = gtk_button_box_new (GTK_ORIENTATION_VERTICAL);
		gtk_container_add (GTK_CONTAINER (window), button_box);

		/* Languages menu */
		widget = gtk_button_new_with_label("Menu");
		g_signal_connect (widget, "clicked", G_CALLBACK (open_menu), NULL);
		gtk_container_add (GTK_CONTAINER (button_box), widget);

		gtk_widget_show_all (window);
	}

    static void close_app (GtkWidget *widget, gpointer data) {
    	_exit_callback();
    }

    static void close_menu (GtkMenuShell *widget, gpointer data) {
    	gtk_menu_shell_deactivate(widget);
    }

    static void open_menu (GtkWidget *widget, gpointer data) {
    	GtkWidget *menu, *submenu, *item;
		menu = gtk_menu_new();
		g_signal_connect (menu, "selection-done", G_CALLBACK (close_menu), NULL);

		submenu = gtk_menu_new();

		item = gtk_check_menu_item_new_with_label("English");
		g_signal_connect (item, "select", G_CALLBACK (setlang_en), NULL);
		gtk_check_menu_item_set_active(item, (settings[SETTING_LANG] == LANG_EN));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		item = gtk_check_menu_item_new_with_label("Francais");
		g_signal_connect (item, "select", G_CALLBACK (setlang_fr), NULL);
		gtk_check_menu_item_set_active(item, (settings[SETTING_LANG] == LANG_FR));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		/* Languages menu */
		item = gtk_menu_item_new_with_label(language_char_str(LANG_STR_LANGUAGE));
		gtk_menu_item_set_submenu(item, submenu);
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		/* Silent mode */
		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_SILENT_ERRS));
		g_signal_connect (item, "select", G_CALLBACK (toggle_silent_mode), NULL);
		gtk_check_menu_item_set_active(item, (settings[SETTING_SILENT]==SETTING_SILENT_ON)); //
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);
		
		/* Auto copy/paste mode */
		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_ENABLEAUTOCOPY));
		g_signal_connect (item, "select", G_CALLBACK (toggle_auto_copypaste), NULL);
		gtk_check_menu_item_set_active(item, (settings[SETTING_AUTOCOPY]==SETTING_AUTOCOPY_ON));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		submenu = gtk_menu_new();

		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_DEGREES));
		g_signal_connect (item, "select", G_CALLBACK (setangle_deg), NULL);
		gtk_check_menu_item_set_active(item, (settings[SETTING_ANGLE] == SETTING_ANGLE_DEG));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		item = gtk_check_menu_item_new_with_label(language_char_str(LANG_STR_RADIANS));
		g_signal_connect (item, "select", G_CALLBACK (setangle_rad), NULL);
		gtk_check_menu_item_set_active(item, (settings[SETTING_ANGLE] == SETTING_ANGLE_RAD));
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), item);

		/* Angles menu */
		item = gtk_menu_item_new_with_label(language_char_str(LANG_STR_ANGLE_UNITS));
		gtk_menu_item_set_submenu(item, submenu);
		gtk_widget_show(item);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), item);

		/* Display menu */
		gtk_menu_popup (GTK_MENU_SHELL (menu), NULL, NULL, NULL, NULL, 0, gtk_get_current_event_time());
		gtk_widget_show(menu);
	}

    static void toggle_silent_mode (GtkWidget *widget, gpointer data) {
    	settings[SETTING_SILENT] = (settings[SETTING_SILENT]==SETTING_SILENT_ON) ? SETTING_SILENT_OFF : SETTING_SILENT_ON;
    }

    static void toggle_auto_copypaste (GtkWidget *widget, gpointer data) {
    	settings[SETTING_AUTOCOPY] = (settings[SETTING_AUTOCOPY]==SETTING_AUTOCOPY_ON) ? SETTING_AUTOCOPY_OFF : SETTING_AUTOCOPY_ON;
    }

	static void setlang_en (GtkWidget *widget, gpointer data) {
		settings[SETTING_LANG] = LANG_EN;
		language_set_current(LANG_EN);
    }

    static void setlang_fr (GtkWidget *widget, gpointer data) {
		settings[SETTING_LANG] = LANG_FR;
		language_set_current(LANG_FR);
	}

    static void setangle_deg (GtkWidget *widget, gpointer data) {
    	settings[SETTING_ANGLE] = SETTING_ANGLE_DEG;
    }

    static void setangle_rad (GtkWidget *widget, gpointer data) {
    	settings[SETTING_ANGLE] = SETTING_ANGLE_RAD;
    }
    
	/** 
	 * Check window messages, process events
	 */
	void update_interface( void ) {
		while (gtk_events_pending ())
			gtk_main_iteration ();
	}
    
	void hotkey_wait( void ) {
		Display* display = XOpenDisplay(0);
    	Window window = DefaultRootWindow(display);
    	XEvent event;

    	KeyCode space = XKeysymToKeycode(display, XK_v);

	   	XGrabKey(display, space, AnyModifier, window, 1, GrabModeSync, GrabModeSync);
    	XNextEvent(display, &event);
    	if (event.type == KeyPress && event.xkey.keycode == space && event.xkey.state & ControlMask) {
    		_parse_callback( from_clipboard() );
    	} else {
    		XAllowEvents(display, ReplayKeyboard, event.xkey.time);
    		XFlush(display);
    	}
	}
	
	/** 
	 * Print help message to stdout
	 */
	void print_help( void ){}
	
	/** 
	 * Add an entry to the menu history
	 * @param entry The string to add
	 */
    void add_history(const wchar_t *entry) {}

	/** 
	 * Copy string to system clipboard
	 * @param entry The string to add
	 */
    void to_clipboard(const wchar_t *entry) {
		GtkClipboard *clipboard = gtk_clipboard_get_default( gdk_display_get_default() );
		const char* text = malloc(sizeof(wchar_t)*(wcslen(entry)+1));
		if (text == NULL) return;

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
			g_free(text);
			return NULL;
		}

		if (mbstowcs(wtext, text, strlen(text)) == strlen(text))
			wtext[strlen(text)] = L'\0';

		g_free(text);
		return wtext;
	}

	/**
	 * Get the path to a valid configuration file
	 * Search in current directory, then relevant home dir
	 *
	 * @return Path to the configuration file
	 */
	const char* config_path( void ){}

	/**
	 * Get the path to a valid configuration file
	 * @param title The title of the error message
	 * @param msg The error message
	 * @param fatal if non 0, exit
	 */
    void error_msg(const wchar_t* title, const wchar_t* msg, char fatal) {}

    /**
     * Get the value of a given setting
     * @param setting The setting to fetch
     *
     * @return The setting's current value. -1 if setting is invalid
     */
    int get_setting(unsigned int setting) {
	if (setting < N_SETTINGS)
	return settings[setting];

        return -1;
    }

    /**
     * Set the value of a given setting
     * @param setting The setting to modify
     * @param value The value to store
     */
    void set_setting(unsigned int setting, int value) {
        if (setting < N_SETTINGS)
	settings[setting] = value;
    }
#endif