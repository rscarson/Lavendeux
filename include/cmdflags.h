#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OPTION_LEN 25
#define MAX_OPTIONHELP_LEN 50
#define HELP_BUFFER_LEN 2048

typedef enum {
	TYPE_DEFAULT,
	TYPE_VALUE
} cmd_flag_type;

typedef struct {
	char str_long[MAX_OPTION_LEN];
	char str_short;

	char help[MAX_OPTIONHELP_LEN];
	void (*handler) ( void );

	cmd_flag_type type;
	char value_name[MAX_OPTION_LEN];

	char present;
	char value[HELP_BUFFER_LEN];
} cmd_flag;

/* Handlers */
void optionhandler_help( void );
void optionhandler_config( void );
void optionhandler_noconfig( void );
void optionhandler_debug( void );

static cmd_flag flags[] = {
	{"help", 'h', "Display help", optionhandler_help, TYPE_DEFAULT, "", 0, ""},
	{"config", 'c', "Use <path> as the configuration file", optionhandler_config, TYPE_VALUE, "path", 0, ""},
	{"no-config", 'n', "Do not load a configuration file; Load defaults", optionhandler_noconfig, TYPE_DEFAULT, "", 0, ""},
	{"debug", 'd', "Start with debug console enabled", optionhandler_debug, TYPE_DEFAULT, "", 0, ""},
};

/* Functions */
char* cmdflag_help();
int cmdflag_present(cmd_flag*, const char*);
void cmdflag_scan(char*);
void cmdflag_run(int, char*[]);
char* cmdflag_value(const char*);