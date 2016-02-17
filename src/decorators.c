#include "hashing.h"
#include "parse.h"
#include "decorators.h"

#include <stdlib.h>
#include <math.h>
#include <wchar.h>
#include <stdio.h>

/**
 * Build the decorator table
 */
void init_decorators( void ) {
	table_create(&decorators, HASH_DEFAULT_SIZE);

	table_put(&decorators, L"u", (void*)decorator_unsigned, NULL);
	table_put(&decorators, L"unsigned", (void*)decorator_unsigned, NULL);

	table_put(&decorators, L"i", (void*)decorator_int, NULL);
	table_put(&decorators, L"int", (void*)decorator_int, NULL);
	table_put(&decorators, L"integer", (void*)decorator_int, NULL);

	table_put(&decorators, L"f", (void*)decorator_float, NULL);
	table_put(&decorators, L"float", (void*)decorator_float, NULL);

	table_put(&decorators, L"sci", (void*)decorator_sci, NULL);
	table_put(&decorators, L"scientific", (void*)decorator_sci, NULL);

	table_put(&decorators, L"b", (void*)decorator_bin, NULL);
	table_put(&decorators, L"bin", (void*)decorator_bin, NULL);
	table_put(&decorators, L"binary", (void*)decorator_bin, NULL);

	table_put(&decorators, L"o", (void*)decorator_oct, NULL);
	table_put(&decorators, L"oct", (void*)decorator_oct, NULL);
	table_put(&decorators, L"octal", (void*)decorator_oct, NULL);

	table_put(&decorators, L"h", (void*)decorator_hex, NULL);
	table_put(&decorators, L"hex", (void*)decorator_hex, NULL);
	table_put(&decorators, L"hexadecimal", (void*)decorator_hex, NULL);
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
	int bit_value = 1;
	int i = 2;
	int_value_t iv;

	int_value(v, &iv);

	/* Prefix */
	decorated[0] = L'0';
	decorated[1] = L'b';

	/* Find starting bit */
	while (iv - bit_value > 0)
		bit_value *= 2;
	if (bit_value > iv)
		bit_value /= 2;
	
	while (bit_value >= 1) {
		if (iv - bit_value >= 0) {
			decorated[i++] = L'1';
			iv -= bit_value;
		} else 
			decorated[i++] = L'0';

		bit_value /= 2;
	}

	decorated[i] = L'\0';
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