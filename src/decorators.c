#include "hashing.h"
#include "parse.h"
#include "decorators.h"

#include <stdlib.h>
#include <math.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>

/**
 * Build the decorator table
 */
int init_decorators( void ) {
	int i, len;
	table_create(&decorators, HASH_DEFAULT_SIZE);

	len = sizeof(decorator_function_declarations) / sizeof(decorator_function_declarations[0]);
	for (i=0; i<len; i++) {
		if (table_put(&decorators, 
			decorator_function_declarations[i].name, 
			(void*) (decorator_function_declarations[i].fn),
			NULL
		) != NO_FAILURE)
			return FAILURE_ALLOCATION;
	}

	return NO_FAILURE;
}

void decorators_destroy( void ) {
	table_destroy(&decorators, NULL);
}

/**
 * Apply a decorator
 * @param name Callable name of the decorator
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 *
 * @return 0 if the operation fails
 */
int decorate(const wchar_t* name, const value* v, wchar_t* decorated) {
	decorator_fn decorator = (decorator_fn) table_get(&decorators, name);
	if (decorator == NULL)
		return 0;
	
	decorator(v, decorated);
	return 1;
}

/**
 * Unsigned integer
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_unsigned(const value* v, wchar_t* decorated) {
	int_value_t iv;

	int_value(v, &iv);
	swprintf(decorated, L"%Lu", iv);
}

/**
 * Integer
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_int(const value* v, wchar_t* decorated) {
	int_value_t iv;

	int_value(v, &iv);
	swprintf(decorated, L"%Ld", iv);
}

/**
 * Floating point
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_float(const value* v, wchar_t* decorated) {
	float_value_t fv;
	int i;

	float_value(v, &fv);
	swprintf(decorated, L"%Lf", fv);

	for (i=wcslen(decorated)-1; i>=1; i--)
		if (decorated[i] == L'0' && decorated[i-1] != L'.')
			decorated[i] = L'\0';
		else break;
}

/**
 * Scientific notation
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_sci(const value* v, wchar_t* decorated) {
	float_value_t fv;
	
	float_value(v, &fv);
	swprintf(decorated, L"%LE", fv);
}

/**
 * Binary
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_bin(const value* v, wchar_t* decorated) {
	wchar_t buffer[EXPRESSION_MAX_LEN];
	int_value_t mask = 1;
	int i = 0;

	/* Value */
	int_value_t iv;
	int_value(v, &iv);

	/* Prefix */
	decorated[0] = L'0';
	decorated[1] = L'b';

	/* Masking */
	while (mask != 0) {
		buffer[i++] = (iv & mask) ? L'1' : L'0';
		mask <<= 1;
	}

	/* Terminator */
	buffer[i] = L'\0';

	/* Readout */
	wcsrev(buffer);
	i = 0;
	while (buffer[i] != L'\0') {
		if (buffer[i] == L'1') break;
		i++;
	} if (buffer[i] == L'\0') i--;

	wcscpy(&decorated[2], &buffer[i]);
}

/**
 * Octal
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_oct(const value* v, wchar_t* decorated) {
	int_value_t iv;
	
	int_value(v, &iv);
	swprintf(decorated, L"0o%Lo", iv);
}

/**
 * Hexadecimal
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_hex(const value* v, wchar_t* decorated) {
	int_value_t iv;
	
	int_value(v, &iv);
	swprintf(decorated, L"0x%Lx", iv);
}