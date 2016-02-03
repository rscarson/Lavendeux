#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "parse.h"
#include "hashing.h"

int parse_init() {
	if (!table_create(variables, HASH_DEFAULT_SIZE))
		return 0;
	if (!table_create(functions, HASH_DEFAULT_SIZE))
		return 0;
}

int parse_equation(const wchar_t* expression, float *value){}

int get_variable(const wchar_t* name, float *value){}

void put_variable(const wchar_t* name, float value){}

int get_function(function fn, float arguments[], float* value){}

void put_function(const wchar_t* name, function fn){}