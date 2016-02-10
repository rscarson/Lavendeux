#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "tab.h"
#include "lex.h"

#include "parse.h"
#include "hashing.h"
#include "builtins.h"

/**
 * Start up the parsing stores
 *
 * @return int The result of the operation
 */
int parse_init() {
	if (!table_create(&variables, HASH_DEFAULT_SIZE))
		return FAILURE_ALLOCATION;
	if (!table_create(&functions, HASH_DEFAULT_SIZE))
		return FAILURE_ALLOCATION;
	if (!init_builtins(&variables))
		return FAILURE_ALLOCATION;

	return NO_FAILURE;
}

/**
 * Get a text representation of an error code
 * @param code The error code
 *
 * @return Error message string
 */
const char* code_to_msg(int code) {
	switch (code) {
		case FAILURE_UNKNOWN:
			return "Unknown error";
		break;

		case NO_FAILURE:
			return "No error";
		break;

		case FAILURE_INVALID_ARGS:
			return "Invalid arguments supplied to function";
		break;

		case FAILURE_INVALID_NAME:
			return "Unrecognized or invalid function or variable name";
		break;

		case FAILURE_SYNTAX_ERROR:
			return "Syntax error";
		break;

		case FAILURE_ALLOCATION:
			return "Failed to allocate memory";
		break;

		case FAILURE_TYPE:
			return "Operation not valid for given type";
		break;
	}

	return "";
}

/**
 * Process an equation and format the response
 * @param equation The expression to solve
 * @param response String buffer pointer to put the result in
 *
 * @return int 0 if failed, non 0 otherwise
 */
extern int yyparse (yyscan_t, int, int*, value*, char[]);
int parse_equation(const wchar_t* equation, value* response){
	char parse_error[255];
	yyscan_t myscanner;

	yylex_init(&myscanner);

	char* equation_mbs = malloc(sizeof(char)*(wcslen(equation)+1));
	if (equation_mbs == NULL) {
		response->sv = malloc(sizeof(wchar_t)*(strlen(code_to_msg(FAILURE_ALLOCATION))+1));
		mbstowcs(response->sv, code_to_msg(FAILURE_ALLOCATION), strlen(code_to_msg(FAILURE_ALLOCATION)));

    	yylex_destroy(myscanner);
		return 0;
	}

	wcstombs(equation_mbs, equation, wcslen(equation));
	yy_scan_string(equation_mbs, myscanner);

	if (yyparse(myscanner, 0, NULL, response, parse_error) == 1) {
		response->sv = malloc(sizeof(wchar_t)*(strlen(parse_error)+1));
		mbstowcs(response->sv, parse_error, strlen(parse_error));

    	yylex_destroy(myscanner);
		return 0;
	}

    yylex_destroy(myscanner);
	return 1;
}

/**
 * Get a variable's value
 * @param name The name of the variable
 * @param dst A pointer to a value to store the result in
 *
 * @return int The result of the operation
 */
int get_variable(const wchar_t* name, value* dst) {
	value *v = table_get(&variables, name);

	if (v != NULL) {
		*dst = *v;
		return NO_FAILURE;
	}

	return FAILURE_INVALID_NAME;
}

/**
 * Store a value in a variable
 * @param name The name of the variable
 * @param src A pointer to the value to store
 *
 * @return int The result of the operation
 */
int put_variable(const wchar_t* name, value* src) {
	if (!table_put(&variables, name, src))
		return FAILURE_ALLOCATION;
	return NO_FAILURE;
}

/**
 * Solve a function
 * @param name The callable name of the function
 * @param args A list of the resolved arguments to pass in
 * @param n_args The length of the args array
 * @param value A pointer to a value that will hold the return value
 *
 * @return int The result of the operation
 */
int solve_function(const wchar_t* name, value args[], int n_args, value* v) {
	int i;
	int parse_result = NO_FAILURE;
	value *argument_backups[n_args];
	function *definition;

	/* Builtin? */
	if (is_builtin(name))
		 return call_builtin(name, args, n_args, v);

	/* Get the definition */
	definition = table_get(&functions, name);
	if (definition == NULL)
		return FAILURE_INVALID_NAME;

	/* Wrong number of arguments> */
	if (n_args != definition->n_args)
		return FAILURE_INVALID_ARGS;

	/* Backup existing values, replace with argument values */
	for (i=0; i<n_args; ++i) {
		get_variable(definition->arguments[i], argument_backups[i]);
		put_variable(definition->arguments[i], &args[i]);
	}

	/* Do the thing */
	if (parse_result == NO_FAILURE)
		parse_result = parse_equation(definition->expression, v);

	/* Clean up values */
	for (i=0; i<n_args; ++i) {
		if (argument_backups[i] == NULL)
			table_remove(&variables, definition->arguments[i], NULL);
		else
			if (put_variable(definition->arguments[i], argument_backups[i]) != NO_FAILURE)
				parse_result = FAILURE_ALLOCATION;
	}

	return parse_result;
}

/**
 * Add a new function
 * @param name The callable name of the function
 * @param definition The function's details
 *
 * @return int The result of the operation
 */
int put_function(const wchar_t* name, function *definition) {
	return table_put(&functions, name, definition);
}

int_value_t ifactorial(int_value_t in) {
	if (in <= 0)
		return 1;

	return ifactorial(in-1) * in;
}

int float_value(const value* v, float_value_t *out) {
	value* resolved = NULL;
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
			if( (result = get_variable(v->sv, resolved)) == NO_FAILURE )
				return float_value(resolved, out);
			else return result;
		break;
	}

	return FAILURE_ALLOCATION;
}

int int_value(const value* v, int_value_t *out) {
	value* resolved = NULL;
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
			if( (result = get_variable(v->sv, resolved)) == NO_FAILURE )
				return int_value(resolved, out);
			else return result;
		break;
	}

	return FAILURE_ALLOCATION;
}

int value_type(const value* v, char* type) {
	value* resolved = NULL;
	int result;

	if (v->type == VALUE_STRING) {
		result = get_variable(v->sv, resolved);
		if (result != NO_FAILURE)
			return result;
		return value_type(resolved, type);
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
	if (error == VALUE_ERROR) {
		v.iv = error;
	}

	return v;
}