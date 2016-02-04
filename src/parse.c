#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "hashing.h"
#include "builtins.h"

/**
 * Start up the parsing stores
 *
 * @return int The result of the operation
 */
int parse_init() {
	if (!table_create(variables, HASH_DEFAULT_SIZE))
		return FAILURE_UNKNOWN;
	if (!table_create(functions, HASH_DEFAULT_SIZE))
		return FAILURE_UNKNOWN;
	return NO_FAILURE;
}

/**
 * Process an equation and format the response
 * @param equation The expression to solve
 * @param response String buffer pointer to put the result in
 *
 * @return int The result of the operation
 */
int parse_equation(const wchar_t* equation, const wchar_t** response){}

/**
 * Process an equation
 * @param equation The expression to solve
 * @param value A pointer to a double to store the result in
 *
 * @return int The result of the operation
 */
int solve_equation(const wchar_t* equation, *double value){}

/**
 * Get a variable's value
 * @param name The name of the variable
 * @param dst A pointer to a double to store the result in
 *
 * @return int The result of the operation
 */
int get_variable(const wchar_t* name, double* dst) {
	double *value = table_get(variables, name, value);

	if (value != NULL) {
		*dst = *value;
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
int put_variable(const wchar_t* name, double* src) {
	if (!table_put(variables, name, src))
		return FAILURE_UNKNOWN;
	return NO_FAILURE;
}

/**
 * Solve a function
 * @param name The callable name of the function
 * @param args A list of the resolved arguments to pass in
 * @param n_args The length of the args array
 * @param value A pointer to a double that will hold the return value
 *
 * @return int The result of the operation
 */
int solve_function(const wchar_t* name, double[] args, int n_args, double* value) {
	int i;
	int parse_result = NO_FAILURE;
	double *argument_backups[n_args];
	function *definition;

	/* Get the definition */
	definition = table_get(functions, name);
	if (definition == NULL)
		return FAILURE_INVALID_NAME;

	/* Wrong number of arguments> */
	if (n_args != definition->n_args)
		return FAILURE_INVALID_ARGUMENTS;

	/* Backup existing values, replace with argument values */
	for (i=0; i<n_args; ++i) {
		get_variable(definition->arguments[i], argument_backups[i]);
		put_variable(definition->arguments[i], args[i]);
	}

	/* Do the thing */
	if (parse_result == NO_FAILURE)
		parse_result = solve_equation(definition->expression, value);

	/* Clean up values */
	for (i=0; i<n_args; ++i) {
		if (argument_backups[i] == NULL)
			table_remove(variables, definition->arguments[i]);
		else
			if (put_variable(definition->arguments[i], argument_backups[i]) != NO_FAILURE)
				parse_result = FAILURE_UNKNOWN;
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
	return table_put(functions, name, definition);
}
