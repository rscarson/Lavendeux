#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "constructs.h"

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

		case VALUE_IDENT:
			if( (result = get_variable(v->sv, &resolved)) == NO_FAILURE )
				return float_value(&resolved, out);
			else return result;
		break;

		case VALUE_STRING:
			return FAILURE_TYPE;
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

		case VALUE_IDENT:
			if( (result = get_variable(v->sv, &resolved)) == NO_FAILURE )
				return int_value(&resolved, out);
			else return result;
		break;

		case VALUE_STRING:
			return FAILURE_TYPE;
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
int value_type(value* v, char* type) {
	value resolved;
	int result;

	if (v->type == VALUE_IDENT) {
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
		*result = value_type((value*) left, &left_type);
		if (*result != NO_FAILURE)
			return VALUE_ERROR;
		
		if (left_type == VALUE_FLOAT)
			return VALUE_FLOAT;

		if (left_type == VALUE_STRING) {
			*result = FAILURE_TYPE;
			return VALUE_ERROR;
		}
	}
	
	if (right != NULL) {
		*result = value_type((value*) right, &right_type);
		if (*result != NO_FAILURE)
			return VALUE_ERROR;
		
		if (right_type == VALUE_FLOAT)
			return VALUE_FLOAT;

		if (right_type == VALUE_STRING) {
			*result = FAILURE_TYPE;
			return VALUE_ERROR;
		}
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