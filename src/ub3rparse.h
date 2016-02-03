#ifndef UB3RPARSE_H
	#define UB3RPARSE_H

	void exit_callback(void);
	void parse_callback(const wchar_t*);
	void setting_callback(int, int);

	/* Parsing stuff */
	//int yyparse();
	//extern FILE *yyin;

	/* Settings */
	char angle_mode;
	char silent_mode;
#endif