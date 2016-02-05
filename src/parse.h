#ifndef PARSE_H
	#define PARSE_H

	#include "hashing.h"

	/* Helper macros */
	#define TO_RADIANS(x) x * PI / 180.0

	#define FAILURE_UNKNOWN			0
	#define NO_FAILURE				1
	#define FAILURE_INVALID_ARGS	2
	#define FAILURE_INVALID_NAME	3
	#define FAILURE_SYNTAX_ERROR	4

	/* Settings */
	char angle_mode;
	char silent_mode;
	
	/* Values of variable types */
	#define VALUE_FLOAT;
	#define VALUE_INT;
	#define VALUE_STRING;
	typedef int_value_t long long int;
	typedef float_value_t long double;
	typedef {
		char type;

		int_value_t iv;
		float_value_t fv;
		const wchar_t* sv;
	} value;

	typedef struct {
		const wchar_t* expression;

		int n_args;
		const wchar_t* arguments[];
	} function;

	/* Defined variables. string->value */
	hash_table variables;

	/* User defined methods. string->function */
	hash_table functions;

	int parse_equation(const wchar_t*, const wchar_t**);
	int solve_equation(const wchar_t*, *value);

	int get_variable(const wchar_t*, value*);
	int put_variable(const wchar_t*, value);

	int solve_function(const wchar_t*, value[], int, value*);
	int put_function(const wchar_t*, function*);

	char float_value(value*, float_value_t*);
	char int_value(value*, int_value_t*);
#endif