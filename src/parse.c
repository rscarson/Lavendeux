#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "language.h"
#include "constructs.h"
#include "builtins.h"
#include "decorators.h"
#include "list.h"

#ifndef YY_TYPEDEF_YY_SCANNER_T
	#define YY_TYPEDEF_YY_SCANNER_T
	typedef void* yyscan_t;
#endif

#include "tab.h"
#include "lex.h"

/**
 * Initialize the parser
 *
 * @return Result of the operation
 */
int parser_init( void ) {
	if (!constructs_init())
		return FAILURE_ALLOCATION;

	if (!builtins_init())
		return FAILURE_ALLOCATION;

	init_decorators();

	return NO_FAILURE;
}

/**
 * Free the parser
 */
void parser_destroy( void ) {
	constructs_destroy();
	decorators_destroy();
	builtins_destroy();
}

/**
 * Process an equation and format the response
 * @param equation The expression to solve
 * @param response value to put the result in
 *
 * @return Result of the operation
 */
extern int yyparse (yyscan_t, value*, int);
int parse_equation(const wchar_t* equation, value* response, int angle_mode){
	char parse_error[EXPRESSION_MAX_LEN];
	wchar_t stored_function[EXPRESSION_MAX_LEN];
	yyscan_t myscanner;

	yylex_init(&myscanner);
	wcscpy(stored_function, L"");

	char* equation_mbs = (char*) malloc(sizeof(char)*(wcslen(equation)+1));
	if (equation_mbs == NULL) {
		wcscpy(response->sv, language_str(LANG_STR_ERR_ALLOCATION));

    	yylex_destroy(myscanner);
		return FAILURE_ALLOCATION;
	}

	wcstombs(equation_mbs, equation, wcslen(equation));
	equation_mbs[wcslen(equation)] = L'\0';
	yy_scan_string(equation_mbs, myscanner);

	if (yyparse(myscanner, response, angle_mode) == 1) {
		free(equation_mbs);
    	yylex_destroy(myscanner);
		return response->iv;
	}

	free(equation_mbs);
    yylex_destroy(myscanner);
	return NO_FAILURE;
}

/**
 * Call a function
 * @param name The name of the function to call
 * @param args Arguments to pass in
 * @param n_args The number of arguments passed
 * @param v Pointer to a value structure we store the result in
 *
 * @return Result of the operation
 */
int call_function(const wchar_t* name, value args[], int n_args, value* v, int angle_mode) {
	int i;
	char type;
	int result;

	/* Resolve all the arguments */
	for (i=0; i<n_args; i++) {
		if ( (result = value_type(&args[0], &type)) != NO_FAILURE )
			return result;
	}

	/* Call builtin */
	if (is_builtin(name)) {
		return call_builtin(name, args, n_args, v, angle_mode);
	}

	return FAILURE_INVALID_NAME;
}