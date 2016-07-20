#ifndef UB3RPARSE_H
	#define UB3RPARSE_H

	#include "interface.h"

	#define ARG_HELP_LONG "--help"
	#define ARG_HELP_SHORT "-h"

	#define ARG_NO_CONFIG_LONG "--no-config"
	#define ARG_DEBUG "--debug"

	#define ARG_CONFIG_PATH_LONG "--config-path"
	#define ARG_CONFIG_PATH_SHORT "-c"

    #define APPLICATION_NAME "Lavendeux"
	#define COPYRIGHT_MSG "Copyright Richard Carson, 2016."
	#define MAJOR_VERSION "0"
	#define MINOR_VERSION "3"
	#define RELEASE_NUMBER "1"
	
	#define HELP_TITLE "About " APPLICATION_NAME
	#define HELP_URL "http://rscarson.github.io/Lavendeux/"
	#define HELP_MSG APPLICATION_NAME " version " MAJOR_VERSION "." MINOR_VERSION "." RELEASE_NUMBER "\n" \
		COPYRIGHT_MSG "\n" \
		"Ctrl-Space to solves selected expressions\n\n" \
		"Commandline options:\n" \
		"--help : Show this message\n" \
		"-h : Short form of --help\n" \
		"--config-path=<path> : Use <path> as the configuration file\n" \
		"-c=<path> : Short form of --config-path\n" \
		"--no-config : Do not use a configuration file\n" \
		"--extensions-log : Log all extension activity in extensions.log\n"

	void parse_argument(const char*);
	void exit_callback(void);
	void parse_callback(const wchar_t*);
	wchar_t* parse_expression(const wchar_t*);
#endif