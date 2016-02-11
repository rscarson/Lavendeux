#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "ub3rparse.h"
#include "language.h"
#include "parse.h"
#include "interface.h"

int main() {
	FILE *config = fopen(config_path(), "r");
	int setting, value;

	init_interface(exit_callback, parse_callback);
	parse_init();

	/* Read in settings */
	if (config != NULL) {
		while (fscanf(config, "%d=%d\n", &setting, &value) == 2)
			set_setting(setting, value);
	}

	while (1) {
		update_interface();
	}

	return 0;
}

void parse_callback(const wchar_t *target) {
	wchar_t* output;
	wchar_t* output_tmp;
	wchar_t line[wcslen(target)+1];
	int i, pos;
	int winline_mode = 0;

	/* Prepare output buffer */
	output = (wchar_t*) malloc(sizeof(wchar_t)*2);
	if (output == NULL)
		error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);
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
					error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);

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
			error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);

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
			error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);
		wcscpy(response, expression);
		return response;
	}

	/* Allocate new string */
	line = (wchar_t*) malloc(sizeof(wchar_t)*(len-i+1));
	if (line == NULL)
		error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);

	/* Copy string portion, and solve it */
	wcscpy(line, &expression[i]);
	result = parse_equation(line, &v);

	/* Add to expression history */
	add_history(line);

	/* Make sure response was ok */
	switch (result) {
		case NO_FAILURE:
		break;
		
		case FAILURE_UNKNOWN:
			error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Something has gone horribly wrong!", 1);
		break;

		case FAILURE_ALLOCATION:
			error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);
		break;
		
		case FAILURE_INVALID_ARGS:
			error_msg(lang_lookup[LANG_STR_SYNTAX_ERR][get_setting(SETTING_LANG)], L"Invalid arguments supplied to function", 0);
		
			response = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen(expression)+1));
			if (response == NULL)
				error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);
			wcscpy(response, expression);
			return response;
		break;
		
		case FAILURE_INVALID_NAME:
			error_msg(lang_lookup[LANG_STR_SYNTAX_ERR][get_setting(SETTING_LANG)], L"Referenced function or variable does not exist", 0);
		
			response = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen(expression)+1));
			if (response == NULL)
				error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);
			wcscpy(response, expression);
			return response;
		break;
		
		case FAILURE_SYNTAX_ERROR:
			error_msg(lang_lookup[LANG_STR_SYNTAX_ERR][get_setting(SETTING_LANG)], v.sv, 0);
		
			response = (wchar_t*) malloc(sizeof(wchar_t)*(wcslen(expression)+1));
			if (response == NULL)
				error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);
			wcscpy(response, expression);
			return response;
		break;
	}

	/* Prepare final container */
	free(line);
	line = (wchar_t*) malloc(sizeof(wchar_t)*(i+wcslen(v.sv)+1));
	if (line == NULL)
		error_msg(lang_lookup[LANG_STR_RUNTIME_ERR][get_setting(SETTING_LANG)], L"Failed to allocate memory!", 1);

	/* Copy response over */
	wcsncpy(line, expression, i);
	line[i] = L'\0';
	wcscat(line, v.sv);

	return line;
}

void exit_callback( void ) {
	/* Open config */
	FILE *config = fopen(config_path(), "w");

	/* Write all settings out */
	fprintf(config, "%d=%d\n", SETTING_ANGLE, get_setting(SETTING_ANGLE));
	fprintf(config, "%d=%d\n", SETTING_SILENT, get_setting(SETTING_SILENT));
	fprintf(config, "%d=%d\n", SETTING_LANG, get_setting(SETTING_LANG));

	/* Close up and leave */
	fclose(config);
	exit(0);
}

