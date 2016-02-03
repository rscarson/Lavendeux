#ifndef INTERFACE_H
	#define INTERFACE_H

    /* Window settings */
    #define WINDOW_TITLE "Ub3rParse"
    #define MAX_EQUATIONS 5
	#define MAX_LEN 255

	/* Application settings */
	#define MAJOR_VERSION 2
	#define MINOR_VERSION 0
	#define RELEASE_NUMBER 0

	/* About settings */
	#define ABOUT_TITLE "About %s"
	#define ABOUT_MSG "%s Version %d.%d.%d \nCtrl-Space to solve equation(s) in clipboard.\n\nCopyright Richard Carson, 2016."
	
	/* Pointer types */
	typedef void (*exitCallback)();
	typedef void (*parseCallback)(const char*);
	typedef void (*settingCallback)(int, int);

	/* Settings available */
	#define SETTING_ANGLE 	0
		#define SETTING_ANGLE_DEG 	0
		#define SETTING_ANGLE_RAD 	1
	#define SETTING_SILENT 	1
		#define SETTING_SILENT_ON  	0
		#define SETTING_SILENT_OFF 	1  

	/** 
	 * Prepare and draw the interface 
	 * @param parse_callback Method to be called on exit
	 * @param parse_callback Method to be called in event of the hotkeys being pressed
	 * @param setting_callback Method to be called in event of a setting changing
	 */
	void init_interface(exitCallback, parseCallback, settingCallback);
	
	/** 
	 * Check window messages, process events
	 */
	void update_interface( void );
	
	/** 
	 * Add an entry to the menu history
	 * @param entry The string to add
	 */
	void add_history(const char*);

	/** 
	 * Copy string to system clipboard
	 * @param entry The string to add
	 */
	void to_clipboard(const char*);

    /** 
	 * Get string from system clipboard
	 * @return const char* The string fetched
	 */
	const char* from_clipboard( void );

	/**
	 * Get the path to a valid configuration file
	 * Search in current directory, then relevant home dir
	 */
	 const char* config_path( void );

#endif