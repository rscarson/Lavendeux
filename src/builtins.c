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
int init_builtins(hash_table variables) {
	if (
		!table_put(variables, BUILTIN_PI, BUILTIN_PI_VALUE) ||
		!table_put(variables, BUILTIN_E, BUILTIN_E_VALUE)
	) return FAILURE_ALLOCATION;

	if (
		!table_put(builtins, builtin_function{builtin_ceil, 1}) ||
		!table_put(builtins, builtin_function{builtin_floor, 1}) ||
		!table_put(builtins, builtin_function{builtin_round, 2}) ||
		!table_put(builtins, builtin_function{builtin_abs, 1}) ||

		!table_put(builtins, builtin_function{builtin_tan, 1}) ||
		!table_put(builtins, builtin_function{builtin_cos, 1}) ||
		!table_put(builtins, builtin_function{builtin_sin, 1}) ||
		!table_put(builtins, builtin_function{builtin_atan, 1}) ||
		!table_put(builtins, builtin_function{builtin_acos, 1}) ||
		!table_put(builtins, builtin_function{builtin_asin, 1}) ||
		!table_put(builtins, builtin_function{builtin_tanh, 1}) ||
		!table_put(builtins, builtin_function{builtin_cosh, 1}) ||
		!table_put(builtins, builtin_function{builtin_sinh, 1}) ||

		!table_put(builtins, builtin_function{builtin_log10, 1}) ||
		!table_put(builtins, builtin_function{builtin_ln, 1}) ||
		!table_put(builtins, builtin_function{builtin_log, 2}) ||

		!table_put(builtins, builtin_function{builtin_sqrt, 1}) ||
		!table_put(builtins, builtin_function{builtin_root, 2})
	) return FAILURE_ALLOCATION;
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

	builtin_function fn = table_get(builtins, name);
	if (fn == NULL)
		return FAILURE_FAILURE_INVALID_NAME;

	if (n_args != fn->n_args)
		return FAILURE_INVALID_ARGS;

	result = fn->fn(args, v);
	return result;
}

/**
 * Determine if a name represents a builtin function
 * @param name Pointer to a value structure we store the result in
 *
 * @return True if the function is a builtin function
 */
int is_builtin(const wchar_t* name) {
	return table_has(builtins, name);
}

/**
 * Floor rounding
 * Param 0: (float) input
 *
 * Return: int
 */
int builtin_floor(value args[], value* result) {
	float_value_type in = float_value(args[0]);

	result->type = VALUE_INT;
	result->iv = floor(in);
}

/**
 * Ceiling rounding
 * Param 0: (float) input
 *
 * Return: int
 */
int builtin_ceil(value args[], value* result) {
	float_value_type in = float_value(args[0]);

	result->type = VALUE_INT;
	result->iv = ceil(in);
}

/**
 * Rounding
 * Param 0: (float) input
 * Param 1: (int) decimals to which we round
 *
 * Return: float
 */
int builtin_round(value args[], value* result) {
	float_value_type base = float_value(args[0]);
	float_value_type precision = int_value(args[1]);

	result->type = VALUE_FLOAT;
	result->fv = round(base * powl(10, precision)) / powl(10, precision);
}

/**
 * Absolute value
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_abs(value args[], value* result) {
	float_value_type in = float_value(args[0]);

	result->type = VALUE_FLOAT;
	result->fv = fabsl(in);
}

/**
 * Tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_tan(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (fmodl(in, M_PI/2.0))
		return FAILURE_INVALID_ARGS;
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = tanl(in);
}

/**
 * Cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_cos(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = cosl(in);
}

/**
 * Sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sin(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = sinl(in);
}

/**
 * Arc tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_atan(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = atanl(in);
}

/**
 * Arc cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_acos(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = acosl(in);
}

/**
 * Arc sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_asin(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = asinl(in);
}

/**
 * Hyperbolic tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_tanh(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = tanhl(in);
}

/**
 * Hyperbolic cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_cosh(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = coshl(in);
}

/**
 * Hyperbolic sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sinh(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (get_setting(SETTING_ANGLE) == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;
	result->fv = sinhl(in);
}

/**
 * Base 10 logarithm
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_log10(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (in == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = log10l(in);
}

/**
 * Natural logarithm
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_ln(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (in == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = logl(in);
}

/**
 * Base n log
 * Param 0: (float) input
 * Param 1: (float) base
 *
 * Return: float
 */
int builtin_log(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	float_value_type base = float_value(args[1]);
	if (in == 0 || base <= 1)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = logl(in) / logl(args[base]);
}

/**
 * Square root
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sqrt(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	if (in < 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = sqrtl(in);
}

/**
 * Base n root
 * Param 0: (float) input
 * Param 0: (float) base
 *
 * Return: float
 */
int builtin_root(value args[], value* result) {
	float_value_type in = float_value(args[0]);
	float_value_type base = float_value(args[1]);
	if (in < 0 || base == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = powl(in, (float_value_type)1/base);
}