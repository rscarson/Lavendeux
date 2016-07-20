#ifndef INTERFACE_H
	#define INTERFACE_H

	/***************************
	 * Interface specs
	 *
	 * ----------------
	 *  < 0 to 5 last equations here >
	 *  < if over MAX_HISTORY_LEN chars, shorten to MAX_HISTORY_LEN chars + HISTORY_SUFFIX >
	 * ----------------
	 *  languages -> Submenu with checkbox entry for each language
	 *  checkbox entry for silent errors
	 *  enable auto-copy/paste
	 *  angle mode -> Submenu with checkbox entry for each mode
	 * ----------------
	 * about -> show dialog with content defined in laveneux.h
	 * exit -> call exit callback
	 *
	 * Capture CTRL-Space events and call parse_callback
	 */

    /* Window settings */
    #define RUNNING_TITLE "Lavendeux is running..."
    #define RUNNING_MSG "CTRL-Space evaluates mathematical expressions"
	#define CONFIG_FILENAME ".lavendeuxsettings"
    #define MAX_EQUATIONS 5
	#define MAX_LEN 255

	/* History settings */
	#define HISTORY_SUFFIX L"[...]"
	#define MAX_HISTORY_LEN 25
	
	/* Pointer types */
	typedef void (*exitCallback)();
	typedef void (*parseCallback)(const wchar_t*);

	/** 
     * Prepare and draw the interface 
     * @param exit_callback Method to be called in event of a quit
     * @param parse_callback Method to be called in event of the hotkeys being pressed
     */
    void init_interface(exitCallback, parseCallback);
	
	/** 
	 * Check window messages, process events
	 */
	void update_interface( void );

	/**
     * Enable debug console
     */
    void debug_enable( void );
	
	/** 
	 * Print help message to stdout
	 */
	void print_help( void );
	
	/** 
	 * Add an entry to the menu history
	 * @param entry The string to add
	 */
	void add_history(const wchar_t*);

	/** 
	 * Copy string to system clipboard
	 * @param entry The string to add
	 */
	void to_clipboard(const wchar_t*);

    /** 
	 * Get string from system clipboard
	 *
	 * @return const wchar_t* The string fetched
	 */
	const wchar_t* from_clipboard( void );

	/**
	 * Get the path to a valid configuration file
	 * Search in current directory, then relevant home dir
	 *
	 * @return Path to the configuration file
	 */
	char* config_path( void );

	/**
	 * Set a prefered config path. Overrides normal settings
	 *
	 * @param path Path to the configuration file
	 */
	void config_set(const char*);

	/**
	 * Get the path to a valid configuration file
	 * @param title The title of the error message
	 * @param msg The error message
	 * @param fatal if non 0, exit
	 */
	void error_msg(const wchar_t*, const wchar_t*, char);
#endif