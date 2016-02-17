#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "language.h"
#include "constructs.h"
#include "decorators.h"
#include "list.h"

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

/**
 * Get floating point value
 * @param v Value
 * @param out Pointer to a float_value_t to hold the value
 *
 * @return Result
 */
int float_value(const value* v, float_value_t *out) {
	value resolved;
	int result;

	switch (v->type) {
		case VALUE_INT:
			*out = (float_value_t) v->iv;
			return NO_FAILURE;
		break;

		case VALUE_FLOAT:
			*out = v->fv;
			return NO_FAILURE;
		break;

		case VALUE_STRING:
			if( (result = get_variable(v->sv, &resolved)) == NO_FAILURE )
				return float_value(&resolved, out);
			else return result;
		break;
	}

	return FAILURE_ALLOCATION;
}

/**
 * Get integer value
 * @param v Value
 * @param out Pointer to a float_value_t to hold the value
 *
 * @return Result
 */
int int_value(const value* v, int_value_t *out) {
	value resolved;
	int result;

	switch (v->type) {
		case VALUE_INT:
			*out = v->iv;
			return NO_FAILURE;
		break;

		case VALUE_FLOAT:
			*out = (int_value_t) v->fv;
			return NO_FAILURE;
		break;

		case VALUE_STRING:
			if( (result = get_variable(v->sv, &resolved)) == NO_FAILURE )
				return int_value(&resolved, out);
			else return result;
		break;
	}

	return FAILURE_ALLOCATION;
}

/**
 * Get the type of a value
 * @param v Value
 * @param type Pointer to a char to hold the type
 *
 * @return Result
 */
int value_type(const value* v, char* type) {
	value resolved;
	int result;

	if (v->type == VALUE_STRING) {
		result = get_variable(v->sv, &resolved);
		if (result != NO_FAILURE)
			return result;

		*v = resolved;
		return value_type(v, type);
	}
		
	*type = v->type;
	return NO_FAILURE;
}

/**
 * Return the expected type of an expression
 * @param left Pointer to left value or NULL
 * @param right Pointer to right value or NULL
 *
 * @return A value type
 */
char expression_type(const value* left, const value* right, int *result) {
	char left_type;
	char right_type;
	*result = NO_FAILURE;
	
	if (left != NULL) {
		*result = value_type(left, &left_type);
		if (*result != NO_FAILURE)
			return VALUE_ERROR;
		
		if (left_type == VALUE_FLOAT)
			return VALUE_FLOAT;
	}
	
	if (right != NULL) {
		*result = value_type(right, &right_type);
		if (*result != NO_FAILURE)
			return VALUE_ERROR;
		
		if (right_type == VALUE_FLOAT)
			return VALUE_FLOAT;
	}

	return VALUE_INT;
}

/* On error, put code in value->iv */
value verify_expression(const value* left, const value* right) {
	value v;
	int error;

	v.type = expression_type(left, right, &error);
	if (v.type == VALUE_ERROR) {
		v.iv = error;
	}

	return v;
}