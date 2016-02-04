#include <math.h>

#include "hashing.h"
#include "parse.h"
#include "builtins.h"

int init_builtins() {
	if (
		table_put(builtins, builtin_function{builtin_ceil, 1}) &&
		table_put(builtins, builtin_function{builtin_floor, 1}) &&
		table_put(builtins, builtin_function{builtin_round, 2}) &&
		table_put(builtins, builtin_function{builtin_abs, 1}) &&

		table_put(builtins, builtin_function{builtin_tan, 1}) &&
		table_put(builtins, builtin_function{builtin_cos, 1}) &&
		table_put(builtins, builtin_function{builtin_sin, 1}) &&
		table_put(builtins, builtin_function{builtin_atan, 1}) &&
		table_put(builtins, builtin_function{builtin_acos, 1}) &&
		table_put(builtins, builtin_function{builtin_asin, 1}) &&
		table_put(builtins, builtin_function{builtin_tanh, 1}) &&
		table_put(builtins, builtin_function{builtin_cosh, 1}) &&
		table_put(builtins, builtin_function{builtin_sinh, 1}) &&

		table_put(builtins, builtin_function{builtin_log10, 1}) &&
		table_put(builtins, builtin_function{builtin_ln, 1}) &&
		table_put(builtins, builtin_function{builtin_log, 2}) &&

		table_put(builtins, builtin_function{builtin_sqrt, 1}) &&
		table_put(builtins, builtin_function{builtin_root, 2})
	) return NO_FAILURE; else return FAILURE_UNKNOWN;
}

int call_builtin(const wchar_t* name, double[] args, int n_args, double* value) {
	int result;

	builtin_function fn = table_get(builtins, name);
	if (fn == NULL)
		return FAILURE_FAILURE_INVALID_NAME;

	if (n_args != fn->n_args)
		return FAILURE_INVALID_ARGS;

	result = fn->fn(args, value);
	return result;
}

int builtin_floor(double[] args, double* result) {
	*result = floor(args[0]);
}

int builtin_ceil(double[] args, double* result) {
	*result = ceil(args[0]);
}

int builtin_round(double[] args, double* result) {
	*result = round(args[0] * pow(10, args[1])) / pow(10, args[1]);
}

int builtin_abs(double[] args, double* result) {
	*result = abs(args[0]);
}

int builtin_tan(double[] args, double* result) {
	*result = tan(args[0]);
}

int builtin_cos(double[] args, double* result) {
	*result = cos(args[0]);
}

int builtin_sin(double[] args, double* result) {
	*result = sin(args[0]);
}

int builtin_atan(double[] args, double* result) {
	*result = atan(args[0]);
}

int builtin_acos(double[] args, double* result) {
	*result = acos(args[0]);
}

int builtin_asin(double[] args, double* result) {
	*result = asin(args[0]);
}

int builtin_tanh(double[] args, double* result) {
	*result = tanh(args[0]);
}

int builtin_cosh(double[] args, double* result) {
	*result = cosh(args[0]);
}

int builtin_sinh(double[] args, double* result) {
	*result = sinh(args[0]);
}

int builtin_log10(double[] args, double* result) {
	*result = log10(args[0]);
}

int builtin_ln(double[] args, double* result) {
	*result = log(args[0]);
}

int builtin_log(double[] args, double* result) {
	*result = tan(args[0]);
}

int builtin_sqrt(double[] args, double* result) {
	*result = sqrt(args[0]);
}

int builtin_root(double[] args, double* result) {
	*result = tan(args[0]);
}