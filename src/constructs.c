#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "constructs.h"
#include "hashing.h"
#include "decorators.h"
#include "builtins.h"

/**
 * Start up the parsing stores
 *
 * @return int The result of the operation
 */
int constructs_init( void ) {
	if (!table_create(&variables, HASH_DEFAULT_SIZE))
		return FAILURE_ALLOCATION;
	if (!table_create(&functions, HASH_DEFAULT_SIZE))
		return FAILURE_ALLOCATION;
	if (!builtins_init(&variables))
		return FAILURE_ALLOCATION;
	init_decorators();

	return NO_FAILURE;
}

void constructs_destroy( void ) {
	table_destroy(&variables, variable_destroy);
	table_destroy(&functions, function_destroy);
	builtins_destroy();
}

void variable_destroy(void* v) {
	free(v);
}

void function_destroy(void* v) {
	int i;
	function* fn = (function*) v;

	free(fn->expression);
	for (i=0; i<fn->n_args; i++)
		free(fn->arguments[i]);
	free(fn->arguments);

	free(fn);
}

/**
 * Get a variable's value
 * @param name The name of the variable
 * @param dst A pointer to a value to store the result in
 *
 * @return int The result of the operation
 */
int get_variable(const wchar_t* name, value* dst) {
	value *v = (value*) table_get(&variables, name);

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
	if (!table_put(&variables, name, src, variable_destroy))
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
	definition = (function*) table_get(&functions, name);
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
	return table_put(&functions, name, definition, function_destroy);
}

/**
 * Remove a stored function from the table
 * @param name The callable name of the function
 */
void function_remove(const wchar_t* name) {
	table_remove(&functions, name, function_destroy);
}