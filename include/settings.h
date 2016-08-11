#ifndef SETTINGS_H
	#define SETTINGS_H

	/* Settings available */
	/* 0 Value should be default */
	enum SETTINGS {
		SETTING_ANGLE,
		SETTING_SILENT,
		SETTING_LANG,
		SETTING_AUTOCOPY,
		SETTING_SILENTSTART,
		SETTING_HOTMOD,
		SETTING_HOTKEY,

		N_SETTINGS
	};
	
	#define SETTING_ANGLE_DEG 	0
	#define SETTING_ANGLE_RAD 	1
		
	#define SETTING_SILENT_ON  	0
	#define SETTING_SILENT_OFF 	1 

	#define SETTING_AUTOCOPY_ON 0
	#define SETTING_AUTOCOPY_OFF 1

	#define SETTING_SILENTSTART_OFF 0
	#define SETTING_SILENTSTART_ON 1

	void init_settings(const char*);
	void save_settings( void );
	int get_setting(unsigned int);
	void set_setting(unsigned int, int);
#endif