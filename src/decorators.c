#include "hashing.h"
#include "parse.h"
#include "decorators.h"

#include <stdlib.h>
#include <math.h>
#include <wchar.h>
#include <string.h>
#include <stdio.h>

/* Blech */
#ifdef _WIN32
	#define swprintf _snwprintf
#endif 

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
int decorate(const wchar_t* name, value v, wchar_t* decorated) {
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
void decorator_unsigned(value v, wchar_t* decorated) {
	int_value_t iv = RESOLVED_VALUE(v);
	swprintf(decorated, EXPRESSION_MAX_LEN, L"%Lu", iv);
}

/**
 * Integer
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_int(value v, wchar_t* decorated) {
	int_value_t iv = RESOLVED_VALUE(v);
	swprintf(decorated, EXPRESSION_MAX_LEN, L"%Ld", iv);
}

/**
 * Floating point
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_float(value v, wchar_t* decorated) {
	float_value_t fv = RESOLVED_VALUE(v);
	int i;

	swprintf(decorated, EXPRESSION_MAX_LEN, L"%Lf", fv);
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
void decorator_sci(value v, wchar_t* decorated) {
	float_value_t fv = RESOLVED_VALUE(v);
	swprintf(decorated, EXPRESSION_MAX_LEN, L"%LE", fv);
}

/**
 * Binary
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_bin(value v, wchar_t* decorated) {
	wchar_t buffer[EXPRESSION_MAX_LEN];
	int_value_t mask = 1;
	int len;
	int i = 0;

	/* Value */
	int_value_t iv = RESOLVED_VALUE(v);

	/* Prefix */
	decorated[0] = L'0';
	decorated[1] = L'b';

	/* Masking */
	while (mask != 0) {
		buffer[i++] = (iv & mask) ? L'1' : L'0';
		mask <<= 1;
	}

	/* Terminator */
	if (i == 0)
		buffer[i++] = L'0';
	buffer[i] = L'\0';

	/* Trim it */
	for (i=wcslen(buffer)-1; i>0; i--)
		if (buffer[i] == L'1') break;
		else buffer[i] = L'\0';

	/* Reverse it */
	len = wcslen(buffer);
	if (len > 1)
		for (i=0; i<len; i++) {
			if (i >= len/2) break;
			buffer[i] ^= buffer[len-1 - i];
			buffer[len-1 - i] ^= buffer[i];
			buffer[i] ^= buffer[len-1 - i];
		}

	wcscpy(&decorated[2], buffer);
}

/**
 * Octal
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_oct(value v, wchar_t* decorated) {
	int_value_t iv = RESOLVED_VALUE(v);
	swprintf(decorated, EXPRESSION_MAX_LEN, L"0o%Lo", iv);
}

/**
 * Hexadecimal
 * @param v Value to apply the decorator to
 * @param decorated Returned string
 */
void decorator_hex(value v, wchar_t* decorated) {
	int_value_t iv = RESOLVED_VALUE(v);
	swprintf(decorated, EXPRESSION_MAX_LEN, L"0x%Lx", iv);
}