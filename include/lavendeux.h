#ifndef UB3RPARSE_H
	#define UB3RPARSE_H

	#include "interface.h"

	#define ARG_HELP_LONG "--help"
	#define ARG_HELP_SHORT "-h"

	#define ARG_NO_CONFIG_LONG "--no-config"

	#define ARG_CONFIG_PATH_LONG "--config-path"
	#define ARG_CONFIG_PATH_SHORT "-c"

	#define ARG_ANGLE_MODE_LONG "--angle-mode"
	#define ARG_ANGLE_MODE_SHORT "-a"
	#define ARG_OPTION_ANGLE_MODE_DEG "degrees"
	#define ARG_OPTION_ANGLE_MODE_RAD "radians"

	#define ARG_SILENT_MODE_LONG "--silent-errors"
	#define ARG_SILENT_MODE_SHORT "-s"
	#define ARG_OPTION_SILENT_MODE_ON "on"
	#define ARG_OPTION_SILENT_MODE_OFF "off"

	#define ARG_LANGUAGE_LONG "--language"
	#define ARG_LANGUAGE_SHORT "-l"
	#define ARG_OPTION_LANGUAGE_EN "en"
	#define ARG_OPTION_LANGUAGE_FR "fr"

    #define APPLICATION_NAME "Lavendeux"
	#define COPYRIGHT_MSG "Copyright Richard Carson, 2016."
	#define MAJOR_VERSION "0"
	#define MINOR_VERSION "1"
	#define RELEASE_NUMBER "1"
	
	#define HELP_TITLE "About " APPLICATION_NAME
	#define HELP_URL "http://rscarson.github.io/Lavendeux/"
	#define HELP_MSG APPLICATION_NAME " version " MAJOR_VERSION "." MINOR_VERSION "." RELEASE_NUMBER "\n" \
		COPYRIGHT_MSG "\n" \
		"Ctrl-Space to solves equation(s) in the clipboard\n\n" \
		"--help : Show this message\n" \
		"-h : Short form of --help\n" \
		"--config-path=<path> : Use <path> as the configuration file\n" \
		"-c=<path> : Short form of --config-path\n" \
		"--no-config : Do not use a configuration file\n" \
		"--angle-mode=<degrees/radians> : Start in a given mode for angles\n" \
		"-a=<degrees/radians> : Short form of --angle-mode\n" \
		"--silent-errors=<on/off> : Start with silen errors on or off\n" \
		"-s=<on/off> : Short form of --silent-errors\n" \
		"--language=<en/fr> : Start in a given language\n" \
		"-l=<en/fr> : Short form of --language\n"

	int parse_argument(const char*);
	void exit_callback(void);
	void parse_callback(const wchar_t*);
	wchar_t* parse_expression(const wchar_t*);
#endif