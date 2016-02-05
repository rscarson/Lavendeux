#ifndef INTERFACE_H
	#define INTERFACE_H

    /* Window settings */
    #define WINDOW_TITLE "Ub3rParse"
	#define CONFIG_FILENAME "/.ub3rsettings"
    #define MAX_EQUATIONS 5
	#define MAX_LEN 255

	/* History settings */
	#define HISTORY_SUFFIX L"[...]"
	#define MAX_HISTORY_LEN 25

	/* Application settings */
	#define MAJOR_VERSION 2
	#define MINOR_VERSION 0
	#define RELEASE_NUMBER 0

	/* About settings */
	#define ABOUT_TITLE "About %s"
	#define ABOUT_MSG "%s Version %d.%d.%d \nCtrl-Space to solve equation(s) in clipboard.\n\nCopyright Richard Carson, 2016."
	
	/* Pointer types */
	typedef void (*exitCallback)();
	typedef void (*parseCallback)(const wchar_t*);

	/* Settings available */
	#define SETTING_ANGLE 	0
		#define SETTING_ANGLE_DEG 	0
		#define SETTING_ANGLE_RAD 	1
	#define SETTING_SILENT 	1
		#define SETTING_SILENT_OFF 	0  
		#define SETTING_SILENT_ON  	1
	#define SETTING_LANG	2 /*Values are language definitions in language.h */

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
	int get_setting(int);

	/**
	 * St the value of a given setting
	 * @param setting The setting to modify
	 * @param value The value to store
	 */
	void set_setting(int, int);
#endif