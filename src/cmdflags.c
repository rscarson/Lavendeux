#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmdflags.h"
#include "interface.h"
#include "language.h"

/**
 * Get help message for defined flags
 * @return Help string
 */
char* cmdflag_help(const char* msg) {
	int i;
	char helpbuf[HELP_BUFFER_LEN];
	int nFlags = sizeof(flags) / sizeof(cmd_flag);

	char* help = malloc(sizeof(char) * ((HELP_BUFFER_LEN * nFlags)+strlen(msg)) );
	if (help == NULL)
		return "";

	strcpy(help, msg);
	for (i=0; i < nFlags; i++) {
		switch (flags[i].type) {
			case TYPE_DEFAULT:
				sprintf(helpbuf, "--%s, -%c: %s\n", flags[i].str_long, flags[i].str_short, flags[i].help);
			break;

			case TYPE_VALUE:
				sprintf(helpbuf, "--%s=<%s>, -%c=<%s>: %s\n", flags[i].str_long, flags[i].value_name, flags[i].str_short, flags[i].value_name, flags[i].help);
			break;
		}

		strcat(help, helpbuf);
	}

	return help;
}

/**
 * Return 1 if arg is an instance of flag
 * @param flag Flag to check for
 * @param arg Argument to scan
 * @return int 1 if true
 */
int cmdflag_present(cmd_flag* flag, const char* arg) {
	char buffer[MAX_OPTION_LEN];
	char short_name;

	switch (flag->type) {
		case TYPE_DEFAULT:
			if (sscanf(arg, "--%s", buffer) == 1 && strcmp(buffer, flag->str_long) == 0)
				return 1;
			else if (sscanf(arg, "-%c", &short_name) == 1 && short_name == flag->str_short)
				return 1;
		break;

		case TYPE_VALUE:
			if (sscanf(arg, "--%[^'=']=%s", buffer, flag->value) == 2 && strcmp(buffer, flag->str_long) == 0)
				return 1;
			else if (sscanf(arg, "-%c=%s", &short_name, flag->value) == 2 && short_name == flag->str_short)
				return 1;
		break;
	}

	return 0;
}

/**
 * Run the flag on a commandline switch, if one exists
 * @param arg The commandline argument to search
 */
void cmdflag_scan(char* arg) {
	int i;
	char buffer[MAX_OPTION_LEN];
	char short_name;
	int nFlags = sizeof(flags) / sizeof(cmd_flag);

	for (i=0; i< nFlags; i++) {
		if (cmdflag_present(&flags[i], arg)) {
			flags[i].present = 1;
			flags[i].handler();
			return;
		}
	}

	error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_UNRECOGNIZED_COMMAND), 1);
}

/**
 * Scan commandline parameters for arguments
 * @param arg The commandline argument to search
 */
void cmdflag_run(int argc, char* argv[]) {
	int i;

	for (i=1; i<argc; i++)
		cmdflag_scan(argv[i]);
}

char* cmdflag_value(const char* long_name) {
	int i;

	for (i=0; i < sizeof(flags) / sizeof(cmd_flag); i++)
		if (strcmp(flags[i].str_long, long_name) == 0)
			return flags[i].value;
	return "";
}
