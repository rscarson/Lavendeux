#ifndef PARSE_H
	#define PARSE_H

	#include "hashing.h"

	#define EXPRESSION_MAX_LEN 255

	/* Helper macros */
	#define TO_RADIANS(x) x * PI / 180.0

	#define FAILURE_UNKNOWN			0
	#define NO_FAILURE				1
	#define FAILURE_INVALID_ARGS	2
	#define FAILURE_INVALID_NAME	3
	#define FAILURE_SYNTAX_ERROR	4
	#define FAILURE_ALLOCATION		5
	#define FAILURE_TYPE			6
	
	/* Values of variable types */
	#define VALUE_ERROR 	0x00
	#define VALUE_FLOAT 	0x01
	#define VALUE_INT 		0x02
	#define VALUE_STRING 	0x03
	typedef long long int int_value_t;
	typedef long double float_value_t;
	typedef struct {
		char type;

		int_value_t iv;
		float_value_t fv;
		wchar_t* sv;
	} value;

	typedef struct {
		const wchar_t* expression;

		int n_args;
		const wchar_t* arguments[];
	} function;

	/* Defined variables. string->value */
	hash_table *variables;

	/* User defined methods. string->function */
	hash_table *functions;

	const char* error_msg(int code);
	const wchar_t* decorate(value*);

	int parse_equation(const wchar_t*, value*);

	int get_variable(const wchar_t*, value*);
	int put_variable(const wchar_t*, value*);

	int solve_function(const wchar_t*, value[], int, value*);
	int put_function(const wchar_t*, function*);

	int_value_t ifactorial(int_value_t in);

	int float_value(value*, float_value_t*);
	int int_value(value*, int_value_t*);
	int value_type(value*, char*);
	char expression_type(value*, value*, int*);
#endif