#include <math.h>

#include "hashing.h"
#include "parse.h"
#include "interface.h"
#include "builtins.h"

/**
 * Populate builtin functions table
 * @param variables Hash table to put variable values into
 *
 * @return Result of the operation
 */
int init_builtins(hash_table *variables) {
	value* builtin_pi;
	value* builtin_e;
	
	table_create(&builtins, HASH_DEFAULT_SIZE);

	builtin_pi = malloc(sizeof(value));
	builtin_e = malloc(sizeof(value));
	if (builtin_pi == NULL || builtin_e == NULL)
		return FAILURE_ALLOCATION;

	builtin_pi->type = VALUE_FLOAT;
	builtin_e->type = VALUE_FLOAT;
	builtin_pi->fv = BUILTIN_PI_VALUE;
	builtin_e->fv = BUILTIN_E_VALUE;

	if (
		!table_put(variables, BUILTIN_PI, builtin_pi) ||
		!table_put(variables, BUILTIN_E, builtin_e)
	) return FAILURE_ALLOCATION;

	if (
		builtin_put(L"ceil", builtin_ceil, 1) != NO_FAILURE ||
		builtin_put(L"floor", builtin_floor, 1) != NO_FAILURE ||
		builtin_put(L"round", builtin_round, 2) != NO_FAILURE ||
		builtin_put(L"abs", builtin_abs, 1) != NO_FAILURE ||

		builtin_put(L"tan", builtin_tan, 1) != NO_FAILURE ||
		builtin_put(L"cos", builtin_cos, 1) != NO_FAILURE ||
		builtin_put(L"sin", builtin_sin, 1) != NO_FAILURE ||
		builtin_put(L"atan", builtin_atan, 1) != NO_FAILURE ||
		builtin_put(L"acos", builtin_acos, 1) != NO_FAILURE ||
		builtin_put(L"asin", builtin_asin, 1) != NO_FAILURE ||
		builtin_put(L"tanh", builtin_tanh, 1) != NO_FAILURE ||
		builtin_put(L"cosh", builtin_cosh, 1) != NO_FAILURE ||
		builtin_put(L"sinh", builtin_sinh, 1) != NO_FAILURE ||

		builtin_put(L"log10", builtin_log10, 1) != NO_FAILURE ||
		builtin_put(L"ln", builtin_ln, 1) != NO_FAILURE ||
		builtin_put(L"log", builtin_log, 2) != NO_FAILURE ||

		builtin_put(L"sqrt,", builtin_sqrt, 1) != NO_FAILURE ||
		builtin_put(L"root,", builtin_root, 2) != NO_FAILURE
	) return FAILURE_ALLOCATION;

	return NO_FAILURE;
}

int builtin_put(const wchar_t* name, builtin fn, int args) {
	builtin_function* entry = malloc(sizeof(builtin_function));
	if (entry == NULL)
		return FAILURE_ALLOCATION;

	entry->fn = fn;
	entry->n_args = args;

	if (!table_put(&builtins, name, entry))
		return FAILURE_ALLOCATION;
	return NO_FAILURE;
}

/**
 * Determine if a function is a builtin
 * @param name The name of the function to call
 *
 * @return 0 if not builtin
 */
int is_builtin(const wchar_t* name) {
	return table_has(&builtins, name);
}

/**
 * Call a builtin function
 * @param name The name of the function to call
 * @param args Arguments to pass in
 * @param n_args The number of arguments passed
 * @param v Pointer to a value structure we store the result in
 *
 * @return Result of the operation
 */
int call_builtin(const wchar_t* name, value args[], int n_args, value* v) {
	int result;

	builtin_function *fn = table_get(&builtins, name);
	if (fn == NULL)
		return FAILURE_INVALID_NAME;

	if (n_args != fn->n_args)
		return FAILURE_INVALID_ARGS;

	result = fn->fn(args, v);
	return result;
}

/**
 * Floor rounding
 * Param 0: (float) input
 *
 * Return: int
 */
int builtin_floor(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	result->type = VALUE_INT;
	result->iv = floor(in);

	return NO_FAILURE;
}

/**
 * Ceiling rounding
 * Param 0: (float) input
 *
 * Return: int
 */
int builtin_ceil(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	result->type = VALUE_INT;
	result->iv = ceil(in);

	return NO_FAILURE;
}

/**
 * Rounding
 * Param 0: (float) input
 * Param 1: (int) decimals to which we round
 *
 * Return: float
 */
int builtin_round(value args[], value* result) {
	float_value_t base;
	float_value(&args[0], &base);

	int_value_t precision;
	int_value(&args[1], &precision);

	result->type = VALUE_FLOAT;
	result->fv = round(base * powl(10, precision)) / powl(10, precision);

	return NO_FAILURE;
}

/**
 * Absolute value
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_abs(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	result->type = VALUE_FLOAT;
	result->fv = fabsl(in);

	return NO_FAILURE;
}

/**
 * Tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_tan(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (fmodl(in, M_PI/2.0))
		return FAILURE_INVALID_ARGS;
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = tanl(in);

	return NO_FAILURE;
}

/**
 * Cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_cos(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = cosl(in);

	return NO_FAILURE;
}

/**
 * Sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sin(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = sinl(in);

	return NO_FAILURE;
}

/**
 * Arc tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_atan(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = atanl(in);

	return NO_FAILURE;
}

/**
 * Arc cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_acos(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = acosl(in);

	return NO_FAILURE;
}

/**
 * Arc sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_asin(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = asinl(in);

	return NO_FAILURE;
}

/**
 * Hyperbolic tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_tanh(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = tanhl(in);

	return NO_FAILURE;
}

/**
 * Hyperbolic cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_cosh(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = coshl(in);

	return NO_FAILURE;
}

/**
 * Hyperbolic sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sinh(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = sinhl(in);

	return NO_FAILURE;
}

/**
 * Base 10 logarithm
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_log10(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (in == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = log10l(in);

	return NO_FAILURE;
}

/**
 * Natural logarithm
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_ln(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (in == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = logl(in);

	return NO_FAILURE;
}

/**
 * Base n log
 * Param 0: (float) input
 * Param 1: (float) base
 *
 * Return: float
 */
int builtin_log(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	float_value_t base;
	float_value(&args[1], &base);

	if (in == 0 || base <= 1)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = logl(in) / logl(base);

	return NO_FAILURE;
}

/**
 * Square root
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sqrt(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	if (in < 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = sqrtl(in);

	return NO_FAILURE;
}

/**
 * Base n root
 * Param 0: (float) input
 * Param 0: (float) base
 *
 * Return: float
 */
int builtin_root(value args[], value* result) {
	float_value_t in;
	float_value(&args[0], &in);

	float_value_t base;
	float_value(&args[1], &base);

	if (in < 0 || base == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = powl(in, (float_value_t)1/base);

	return NO_FAILURE;
}