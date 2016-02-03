#ifndef UB3RPARSE_H
	#define UB3RPARSE_H

	void exit_callback(void);
	void parse_callback(const char*);
	void setting_callback(int, int);

	/* Settings */
	char error_mode;
	char silent_mode;
#endif