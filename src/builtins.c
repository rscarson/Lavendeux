#include <math.h>
#include <fenv.h>
#include <wchar.h>

#include "hashing.h"
#include "interface.h"
#include "constructs.h"
#include "parse.h"
#include "builtins.h"
#include "settings.h"

#ifdef _WIN32
	#define swprintf _snwprintf
#endif 

/**
 * Populate builtin functions table
 * @param variables Hash table to put variable values into
 *
 * @return Result of the operation
 */
int builtins_init( void ) {
	int i;
	int len;
	
	table_create(&builtins, HASH_DEFAULT_SIZE);

	/* Now functions */
	len = sizeof(builtin_function_declarations) / sizeof(builtin_function_declarations[0]);
	for (i=0; i<len; i++) {
		if (builtin_put(builtin_function_declarations[i].name, builtin_function_declarations[i].fn, builtin_function_declarations[i].args) != NO_FAILURE)
			return FAILURE_ALLOCATION;
	}

	return NO_FAILURE;
}

/**
 * Destroy a builtin function entry
 * @param ptr Pointer to the entry
 */
void builtin_destroy(void* ptr) {
	free(ptr);
}

/**
 * Destroy the builtins table
 */
void builtins_destroy( void ) {
	table_destroy(&builtins, builtin_destroy);
}

/**
 * Add a builtin function to the table
 * @param name The callable name of the function
 * @param fn The function pointer
 * @param args The number of arguments expected
 */
int builtin_put(const wchar_t* name, builtin fn, int args) {
	builtin_function* entry = (builtin_function*) malloc(sizeof(builtin_function));
	if (entry == NULL)
		return FAILURE_ALLOCATION;

	entry->fn = fn;
	entry->n_args = args;

	if (!table_put(&builtins, name, entry, NULL))
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
 * @param args Arguments to pass in. Must be pre-resolved
 * @param n_args The number of arguments passed
 * @param v Pointer to a value structure we store the result in
 *
 * @return Result of the operation
 */
int call_builtin(const wchar_t* name, value args[], int n_args, value* v, int angle_mode) {
	int result;

	/* Fetch function pointer */
	builtin_function *fn = (builtin_function*) table_get(&builtins, name);
	if (fn == NULL)
		return FAILURE_INVALID_NAME;

	/* Check arguments */
	if (n_args != fn->n_args)
		return FAILURE_INVALID_ARGS;

	/* Call it */
	result = fn->fn(args, v, angle_mode);
	return result;
}

/**
 * Floor rounding
 * Param 0: (float) input
 *
 * Return: int
 */
int builtin_floor(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

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
int builtin_ceil(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

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
int builtin_round(value args[], value* result, int angle_mode) {
	float_value_t base = RESOLVED_VALUE(args[0]);
	int_value_t precision = RESOLVED_VALUE(args[1]);

	result->type = VALUE_FLOAT;
	result->fv = round(base * ((float_value_t) powl(10, precision))) / ((float_value_t) powl(10, precision));

	return NO_FAILURE;
}

/**
 * Absolute value
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_abs(value args[], value* result, int angle_mode) {
	value in = args[0];
	switch (in.type) {
		case VALUE_FLOAT:
			result->type = VALUE_FLOAT;
			result->fv = fabsl(in.fv);
		break;

		case VALUE_INT:
			result->type = VALUE_INT;
			result->iv = llabs(in.iv);
		break;
	}

	return NO_FAILURE;
}

/**
 * Tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_tan(value args[], value* result, int angle_mode) {
	float_value_t domain_test;
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	/* Test for domain errors */
	domain_test = fabsl(tanl(PI/2) / tanl(in));
	if (domain_test > 0.99 && domain_test < 1.00 && domain_test != INFINITY)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = tanl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_cos(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = cosl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sin(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = sinl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Arc tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_atan(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = atanl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Arc cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_acos(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = acosl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Arc sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_asin(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = asinl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Hyperbolic tangent
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_tanh(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = tanhl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Hyperbolic cosine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_cosh(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = coshl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Hyperbolic sine
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_sinh(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (angle_mode == SETTING_ANGLE_DEG)
		in = TO_RADIANS(in);

	result->type = VALUE_FLOAT;

	feclearexcept (FE_ALL_EXCEPT);
	result->fv = sinhl(in);
	if (fetestexcept (FE_INVALID))
		return FAILURE_INVALID_ARGS;

	return NO_FAILURE;
}

/**
 * Base 10 logarithm
 * Param 0: (float) input
 *
 * Return: float
 */
int builtin_log10(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (in <= 0)
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
int builtin_ln(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

	if (in <= 0)
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
int builtin_log(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);
	float_value_t base = RESOLVED_VALUE(args[1]);

	if (in <= 0 || base <= 1)
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
int builtin_sqrt(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);

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
int builtin_root(value args[], value* result, int angle_mode) {
	float_value_t in = RESOLVED_VALUE(args[0]);
	float_value_t base = RESOLVED_VALUE(args[1]);

	if (in < 0 || base == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_FLOAT;
	result->fv = powl(in, (float_value_t)1/base);

	return NO_FAILURE;
}

/**
 * String length
 * Param 0: (string) input
 *
 * Return: int
 */
int builtin_strlen(value args[], value* result, int angle_mode) {
	if (args[0].type != VALUE_STRING)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_INT;
	result->iv = wcslen(args[0].sv);
	return NO_FAILURE;
}

/**
 * Substring
 * Param 0: (string) input
 * Param 1: (int) start
 * Param 2: (int) len
 *
 * Return: int
 */
int builtin_substr(value args[], value* result, int angle_mode) {
	if (args[0].type != VALUE_STRING || 
		args[1].type != VALUE_INT || args[1].iv >= wcslen(args[0].sv) ||
		args[2].type != VALUE_INT || args[2].iv < 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_STRING;
	wcsncpy(result->sv, &args[0].sv[args[1].iv], args[2].iv);
	result->sv[args[2].iv] = L'\0';
	return NO_FAILURE;
}

/**
 * Char from int
 * Param 0: (int) index
 *
 * Return: int
 */
int builtin_chr(value args[], value* result, int angle_mode) {
	if (args[0].type != VALUE_INT)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_STRING;
	swprintf(result->sv, EXPRESSION_MAX_LEN, L"%c", args[0].iv);
	result->sv[1] = L'\0';
	return NO_FAILURE;
}

/**
 * Int from char
 * Param 0: (string) character
 *
 * Return: int
 */
int builtin_ord(value args[], value* result, int angle_mode) {
	if (args[0].type != VALUE_STRING || wcslen(args[0].sv) == 0)
		return FAILURE_INVALID_ARGS;

	result->type = VALUE_INT;
	result->iv = args[0].sv[0];
	return NO_FAILURE;
}
