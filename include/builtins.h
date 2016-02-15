#ifndef BUILTINS_H
	#define BUILTINS_H

	#include "hashing.h"
	#include <math.h>

	#define PI 	3.14159265358979
	#define E 	2.71828182845905

	/* A builtin function looks like this */
	typedef int(*builtin)(value[], value*);
	typedef struct {
		builtin fn;
		int n_args;
	} builtin_function;

	/* Store builtin functions */
	hash_table builtins;

	int builtins_init(hash_table*);
	void builtin_destroy(void*);
	void builtins_destroy( void );
	int builtin_put(const wchar_t*, builtin, int);
	int is_builtin(const wchar_t*);
	int call_builtin(const wchar_t*, value[], int, value*);

	/* Builtins */
	int builtin_floor(value[], value*);
	int builtin_ceil(value[], value*);
	int builtin_round(value[], value*);
	int builtin_abs(value[], value*);

	int builtin_tan(value[], value*);
	int builtin_cos(value[], value*);
	int builtin_sin(value[], value*);
	int builtin_atan(value[], value*);
	int builtin_acos(value[], value*);
	int builtin_asin(value[], value*);
	int builtin_rtan(value[], value*);
	int builtin_rcos(value[], value*);
	int builtin_rsin(value[], value*);
	int builtin_tanh(value[], value*);
	int builtin_cosh(value[], value*);
	int builtin_sinh(value[], value*);

	int builtin_log10(value[], value*);
	int builtin_ln(value[], value*);
	int builtin_log(value[], value*);

	int builtin_sqrt(value[], value*);
	int builtin_root(value[], value*);
#endif