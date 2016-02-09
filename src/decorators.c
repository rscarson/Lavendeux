#include "hashing.h"
#include "parse.h"
#include "decorators.h"

#include <stdlib.h>
#include <wchar.h>

void init_decorators( void ) {
	table_create(decorators, HASH_DEFAULT_SIZE);

	table_put(decorators, L"i", decorator_int);
	table_put(decorators, L"int", decorator_int);
	table_put(decorators, L"integer", decorator_int);

	table_put(decorators, L"f", decorator_float);
	table_put(decorators, L"float", decorator_float);

	table_put(decorators, L"o", decorator_oct);
	table_put(decorators, L"oct", decorator_oct);
	table_put(decorators, L"octal", decorator_oct);

	table_put(decorators, L"h", decorator_hex);
	table_put(decorators, L"hex", decorator_hex);
	table_put(decorators, L"hexadecimal", decorator_hex);
}

void decorate(const wchar_t* name, const value* v, wchar_t* decorated) {
	decorator_fn decorator = table_get(decorators, name);
	if (decorator == NULL)
		return;
	decorator(v, decorated);
}

void decorator_int(const value* v, wchar_t* decorated) {
	int_value_t iv;

	int_value(v, &iv);
	swprintf(decorated, L"%Ld", iv);
}

void decorator_float(const value* v, wchar_t* decorated) {
	float_value_t fv;
	
	float_value(v, &fv);
	swprintf(decorated, L"%Lf", fv);
}

void decorator_oct(const value* v, wchar_t* decorated) {
	int_value_t iv;
	
	int_value(v, &iv);
	swprintf(decorated, L"0o%Lo", iv);
}

void decorator_hex(const value* v, wchar_t* decorated) {
	int_value_t iv;
	
	int_value(v, &iv);
	swprintf(decorated, L"0x%Lx", iv);
}