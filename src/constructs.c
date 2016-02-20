#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "constructs.h"
#include "hashing.h"

/**
 * Start up the parsing stores
 *
 * @return int The result of the operation
 */
int constructs_init( void ) {
	value* val;
	int i;
	int len;
	
	if (!table_create(&variables, HASH_DEFAULT_SIZE))
		return FAILURE_ALLOCATION;

	/* Add predefined values */
	len = sizeof(builtin_variable_declarations) / sizeof(builtin_variable_declarations[0]);
	for (i=0; i<len ; i++) {
		/* Allocate the new value */
		val = (value*) malloc(sizeof(value));
		if (val == NULL)
			return FAILURE_ALLOCATION;

		/* Add it to the table */
		*val = builtin_variable_declarations[i].v;
		if (put_variable(builtin_variable_declarations[i].name, val) != NO_FAILURE)
			return FAILURE_ALLOCATION;
	}

	return NO_FAILURE;
}

/**
 * Destroy the construct tables
 */
void constructs_destroy( void ) {
	table_destroy(&variables, variable_destroy);
}

/**
 * Destroy a variable pointer
 * @param v Pointer to the variable
 */
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