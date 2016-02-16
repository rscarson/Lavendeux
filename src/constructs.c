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
	if (!builtins_init(&variables))
		return FAILURE_ALLOCATION;
	init_decorators();

	return NO_FAILURE;
}

void constructs_destroy( void ) {
	table_destroy(&variables, variable_destroy);
	builtins_destroy();
}

void variable_destroy(void* v) {
	free(v);
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