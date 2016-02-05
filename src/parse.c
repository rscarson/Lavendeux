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
		return FAILURE_ALLOCATION;
	if (!table_create(functions, HASH_DEFAULT_SIZE))
		return FAILURE_ALLOCATION;
	if (!init_builtins(variables))
		return FAILURE_ALLOCATION;

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
 * @param value A pointer to a value to store the result in
 *
 * @return int The result of the operation
 */
int solve_equation(const wchar_t* equation, *value value){}

/**
 * Get a variable's value
 * @param name The name of the variable
 * @param dst A pointer to a value to store the result in
 *
 * @return int The result of the operation
 */
int get_variable(const wchar_t* name, value* dst) {
	value *value = table_get(variables, name, value);

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
int put_variable(const wchar_t* name, value* src) {
	if (!table_put(variables, name, src))
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
int solve_function(const wchar_t* name, value[] args, int n_args, value* value) {
	int i;
	int parse_result = NO_FAILURE;
	value *argument_backups[n_args];
	function *definition;

	/* Builtin? */
	if (is_builtin(name))
		 return call_builtin(name, args, n_args, value);

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
	return table_put(functions, name, definition);
}

char float_value(value* v, float_value_t *out) {
	value* resolved;
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

char int_value(value* v, int_value_t *out) {
	value* resolved;
	
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