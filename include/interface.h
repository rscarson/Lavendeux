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
    #define RUNNING_MSG "Lavendeux is running..."
	#define CONFIG_FILENAME "/.lavendeuxsettings"
    #define MAX_EQUATIONS 5
	#define MAX_LEN 255

	/* History settings */
	#define HISTORY_SUFFIX L"[...]"
	#define MAX_HISTORY_LEN 25
	
	/* Pointer types */
	typedef void (*exitCallback)();
	typedef void (*parseCallback)(const wchar_t*);

	/* Settings available */
	/* 0 Value should be default */
	enum SETTINGS {
		SETTING_ANGLE,
		SETTING_SILENT,
		SETTING_LANG,
		SETTING_AUTOCOPY,

		N_SETTINGS
	};
	
	#define SETTING_ANGLE_DEG 	0
	#define SETTING_ANGLE_RAD 	1
		
	#define SETTING_SILENT_ON  	0
	#define SETTING_SILENT_OFF 	1 

	#define SETTING_AUTOCOPY_ON 0
	#define SETTING_AUTOCOPY_OFF 1

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
	const char* config_path( void );

	/**
	 * Get the path to a valid configuration file
	 * @param title The title of the error message
	 * @param msg The error message
	 * @param fatal if non 0, exit
	 */
	void error_msg(const wchar_t*, const wchar_t*, char);

	/**
	 * Get the value of a given setting
	 * @param setting The setting to fetch
	 *
	 * @return The setting's current value. -1 if setting is invalid
	 */
	int get_setting(unsigned int);

	/**
	 * Set the value of a given setting
	 * @param setting The setting to modify
	 * @param value The value to store
	 */
	void set_setting(unsigned int, int);
#endif