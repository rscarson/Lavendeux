#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <windows.h>

#include "lavendeux.h"
#include "language.h"
#include "parse.h"
#include "interface.h"
#include "extensions.h"

/* Configuration file settings */
int config_off = 0;
FILE* config = NULL;

int main(int argc, char* argv[]) {
	int setting, value;

	/* Start UI */
	init_interface(exit_callback, parse_callback);
	parser_init();

	/* Commandline arguments */
	for (; argc>1; argc--) {
		parse_argument(argv[argc-1]);
	}

	/* Config */
	if (config == NULL && !config_off) {
		config = fopen(config_path(), "r");
		if (config == NULL)
			printf("Could not open '%s': %s", config_path(), strerror(errno));
	}

	/* Read in settings */
	if (config != NULL) {
		printf("Reading config\n");
		while (fscanf(config, " %d = %d \n", &setting, &value) == 2) {
			if (setting < N_SETTINGS)
				set_setting(setting, value);
		}

		fclose(config);
	}

	/* Main update loop */
	while (1) {
		update_interface();
	}

	return 0;
}

/**
 * Process a commandline argument
 * @param arg The argument string
 */
void parse_argument(const char* arg) {
	/* Help */
	if (strcmp(arg, ARG_HELP_LONG) == 0 || strcmp(arg, ARG_HELP_SHORT) == 0) {
		print_help();
		return;
	}

	/* No config */
	if (strncmp(arg, ARG_NO_CONFIG_LONG, strlen(ARG_NO_CONFIG_LONG)) == 0) {
		config_off = 1;
		return;
	}

	/* Debug mode */
	if (strncmp(arg, ARG_DEBUG, strlen(ARG_DEBUG)) == 0) {
		debug_enable();
		return;
	}

	/* Config path */
	if (strncmp(arg, ARG_CONFIG_PATH_LONG, strlen(ARG_CONFIG_PATH_LONG)) == 0 || strncmp(arg, ARG_CONFIG_PATH_SHORT, strlen(ARG_CONFIG_PATH_SHORT)) == 0) {
		while (*arg != '\0')
			if (*arg == '=') {
				arg++;
				break;
			} else arg++;

		config = fopen(arg, "w+");
		if (config == NULL)
			error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_CONFIG), 1);
		return;
	}
	
	error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_UNRECOGNIZED_COMMAND), 1);
}

/**
 * Process a callback from the interface
 * @param target The input string
 */
void parse_callback(const wchar_t *target) {
	wchar_t* output;
	wchar_t* output_tmp;
	wchar_t line[wcslen(target)+1];
	unsigned int i, pos;
	int winline_mode = 0;
	printf("Received a callback. Target string: %S\n", target);

	/* Prepare output buffer */
	output = (wchar_t*) malloc(sizeof(wchar_t)*2);
	if (output == NULL)
		error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);
	wcscpy(output, L"");

	/* Scan line by line */
	for (i=pos=0; i<wcslen(target); ++i) {
		switch (target[i]) {
			/* Line complete */
			case L'\n':
				/* Line NULL terminator */
				line[pos] = L'\0';
				pos = 0;

				/* Get line output */
				output_tmp = parse_expression(line);

				/* Expand output buffer */
				output = (wchar_t*) realloc(output, sizeof(wchar_t)*( wcslen(output_tmp) + wcslen(output) + 3) /* Space for carriage return and line break */);
				if (output == NULL)
					error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);

				/* Might need a carriage return here */
				if (winline_mode)
					wcscat(output, L"\r");
				wcscat(output, L"\n");

				/* Append to output */
				wcscat(output, output_tmp);
				free(output_tmp);
			break;

			/* Skip this, and use windows breaks */
			case L'\r':
				winline_mode = 1;
			break;

			/* Copy next line character */
			default:
				line[pos++] = target[i];
		}
	}

	/* Parse the trailing line too */
	if (pos > 0) {
		line[pos] = L'\0';
		output_tmp = parse_expression(line);

		/* Expand output buffer */
		output = (wchar_t*) realloc(output, sizeof(wchar_t)*( wcslen(output_tmp) + wcslen(output) + 3) /* Space for carriage return and line break */);
		if (output == NULL)
			error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);

		/* Might need a carriage return here */
		if (winline_mode)
			wcscat(output, L"\r");
		wcscat(output, L"\n");

		/* Append to output */
		wcscat(output, output_tmp);
		free(output_tmp);
	}

	/* Remove prefixing line breaks */
	while (output[0] == L'\r' || output[0] == L'\n')
		output = &output[1];

	/* Final output */
	to_clipboard(output);
	free(output);
}

/**
 * Process a callback from the interface
 * @param expression The input formula
 *
 * @return The result
 */
wchar_t* parse_expression(const wchar_t* expression) {
	wchar_t* line;
	wchar_t* response;
	value v;
	int i, len, result;

	/* Get string length */
	len = wcslen(expression);

	/* Scan for first non-whitespace character in the line */
	for (i=0; i<len; ++i)
		if (expression[i] != L' ' && expression[i] != L'\t')
			break;

	/* Empty line. Abort. */
	if (i == len) {
		response = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen(expression)+1));
		if (response == NULL)
			error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);
		wcscpy(response, expression);
		return response;
	}

	/* Allocate new string */
	line = (wchar_t*) malloc(sizeof(wchar_t)*(len-i+1));
	if (line == NULL)
		error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);

	/* Copy string portion, and solve it */
	wcscpy(line, &expression[i]);
	result = parse_equation(line, &v, get_setting(SETTING_ANGLE));

	/* Add to expression history */
	add_history(line);

	/* Make sure response was ok */
	if (result == FAILURE_UNKNOWN || result == FAILURE_ALLOCATION)
		error_msg(language_str(LANG_STR_RUNTIME_ERR), v.sv, 1);
	else if (result != NO_FAILURE) {
		error_msg(language_str(LANG_STR_SYNTAX_ERR), v.sv, 0);
		
		free(line);
		return (wchar_t*) expression;
	}
	/* Prepare final container */
	free(line);
	line = (wchar_t*) malloc(sizeof(wchar_t)*(i+wcslen(v.sv)+1));
	if (line == NULL)
		error_msg(language_str(LANG_STR_RUNTIME_ERR), language_str(LANG_STR_ERR_ALLOCATION), 1);

	/* Copy response over */
	wcsncpy(line, expression, i);
	line[i] = L'\0';
	wcscat(line, v.sv);

	return line;
}

/**
 * Callback to exit
 */
void exit_callback( void ) {
	/* Free up memory */
	printf("Exiting...\n");
	parser_destroy();

	/* Open config */
	if (!config_off) {
		printf("Writing config.\n");
		config = fopen(config_path(), "w+");
		if (config != NULL) {
			/* Write all settings out */
			fprintf(config, "%d=%d\n", SETTING_ANGLE, get_setting(SETTING_ANGLE));
			fprintf(config, "%d=%d\n", SETTING_SILENT, get_setting(SETTING_SILENT));
			fprintf(config, "%d=%d\n", SETTING_LANG, get_setting(SETTING_LANG));
			fprintf(config, "%d=%d\n", SETTING_AUTOCOPY, get_setting(SETTING_AUTOCOPY));

			/* Close up and leave */
			fclose(config);
		}
	}

	exit(0);
}

