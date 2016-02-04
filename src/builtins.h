#ifndef BUILTINS_H
	#define BUILTINS_H

	/* A builtin function looks like this */
	typedef int(*builtin)(double[], double*);
	typedef struct {
		builtin fn;
		int n_args;
	} builtin_function;

	/* Store builtin functions */
	hash_table builtins;

	int init_builtins();
	int call_builtin(const wchar_t*, double[], int, double*);

	/* Builtins */
	int builtin_floor(double[], double*);
	int builtin_ceil(double[], double*);
	int builtin_round(double[], double*);
	int builtin_abs(double[], double*);

	int builtin_tan(double[], double*);
	int builtin_cos(double[], double*);
	int builtin_sin(double[], double*);
	int builtin_atan(double[], double*);
	int builtin_acos(double[], double*);
	int builtin_asin(double[], double*);
	int builtin_rtan(double[], double*);
	int builtin_rcos(double[], double*);
	int builtin_rsin(double[], double*);
	int builtin_tanh(double[], double*);
	int builtin_cosh(double[], double*);
	int builtin_sinh(double[], double*);

	int builtin_log10(double[], double*);
	int builtin_ln(double[], double*);
	int builtin_log(double[], double*);

	int builtin_sqrt(double[], double*);
	int builtin_root(double[], double*);
#endif