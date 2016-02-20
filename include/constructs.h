#ifndef CONSTRUCTS_H
	#define CONSTRUCTS_H

	#include "parse.h"
	#include "hashing.h"

	#define PI 	3.14159265358979
	#define E 	2.71828182845905

	/* Defined variables. string->value */
	hash_table variables;

	/* User defined methods. string->function */
	hash_table functions;

	int constructs_init( void );
	void constructs_destroy( void );
	
	void variable_destroy(void*);

	int get_variable(const wchar_t*, value*);
	int put_variable(const wchar_t*, value*);

	/* For static table entries */
	typedef struct { const wchar_t* name; value v; } static_builtin_var;

	/* Builtin value definitions */
	static const static_builtin_var builtin_variable_declarations[] = {
		{ L"pi", { VALUE_FLOAT, 0, PI } },
		{ L"e", { VALUE_FLOAT, 0, E } },
	};
#endif