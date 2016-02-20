#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "test.h"
#include "decorators.h"
#include "parse.h"

static const value value_zero = {VALUE_INT, 0};
static const value value_small = {VALUE_FLOAT, 0, 0.12345};
static const value value_big = {VALUE_FLOAT, 0, 12345.12345};
static const value value_255 = {VALUE_INT, 255};
static const value value_neg = {VALUE_INT, -255};

int init_tests( void ) {
	ASSERT_EQUAL(NO_FAILURE, init_decorators());
	return 0;
}

int destroy_tests( void ) {
	decorators_destroy();
	return 0;
}

int test_unsigned( void ) {
	wchar_t result[EXPRESSION_MAX_LEN];

	ASSERT_EQUAL(NO_FAILURE, decorate(L"u", value_zero, result));
	ASSERT_WSTR_EQUAL(L"0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"u", value_small, result));
	ASSERT_WSTR_EQUAL(L"0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"u", value_big, result));
	ASSERT_WSTR_EQUAL(L"12345", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"u", value_255, result));
	ASSERT_WSTR_EQUAL(L"255", result);

	return 0;
}

int test_int( void ) {
	wchar_t result[EXPRESSION_MAX_LEN];

	ASSERT_EQUAL(NO_FAILURE, decorate(L"i", value_zero, result));
	ASSERT_WSTR_EQUAL(L"0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"i", value_small, result));
	ASSERT_WSTR_EQUAL(L"0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"i", value_big, result));
	ASSERT_WSTR_EQUAL(L"12345", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"i", value_255, result));
	ASSERT_WSTR_EQUAL(L"255", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"i", value_neg, result));
	ASSERT_WSTR_EQUAL(L"-255", result);
	
	return 0;
}

int test_float( void ) {
	wchar_t result[EXPRESSION_MAX_LEN];

	ASSERT_EQUAL(NO_FAILURE, decorate(L"f", value_zero, result));
	ASSERT_WSTR_EQUAL(L"0.0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"f", value_small, result));
	ASSERT_WSTR_EQUAL(L"0.12345", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"f", value_big, result));
	ASSERT_WSTR_EQUAL(L"12345.12345", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"f", value_255, result));
	ASSERT_WSTR_EQUAL(L"255.0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"f", value_neg, result));
	ASSERT_WSTR_EQUAL(L"-255.0", result);
	
	return 0;
}

int test_bin( void ) {
	wchar_t result[EXPRESSION_MAX_LEN];

	ASSERT_EQUAL(NO_FAILURE, decorate(L"b", value_zero, result));
	ASSERT_WSTR_EQUAL(L"0b0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"b", value_small, result));
	ASSERT_WSTR_EQUAL(L"0b0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"b", value_big, result));
	ASSERT_WSTR_EQUAL(L"0b11000000111001", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"b", value_255, result));
	ASSERT_WSTR_EQUAL(L"0b11111111", result);
	
	return 0;
}

int test_oct( void ) {
	wchar_t result[EXPRESSION_MAX_LEN];

	ASSERT_EQUAL(NO_FAILURE, decorate(L"o", value_zero, result));
	ASSERT_WSTR_EQUAL(L"0o0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"o", value_small, result));
	ASSERT_WSTR_EQUAL(L"0o0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"o", value_big, result));
	ASSERT_WSTR_EQUAL(L"0o30071", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"o", value_255, result));
	ASSERT_WSTR_EQUAL(L"0o377", result);
	
	return 0;
}

int test_hex( void ) {
	wchar_t result[EXPRESSION_MAX_LEN];

	ASSERT_EQUAL(NO_FAILURE, decorate(L"h", value_zero, result));
	ASSERT_WSTR_EQUAL(L"0x0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"h", value_small, result));
	ASSERT_WSTR_EQUAL(L"0x0", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"h", value_big, result));
	ASSERT_WSTR_EQUAL(L"0x3039", result);

	ASSERT_EQUAL(NO_FAILURE, decorate(L"h", value_255, result));
	ASSERT_WSTR_EQUAL(L"0xff", result);
	
	return 0;
}


int main() {
	/* Set ourselves up */
	TEST_SETUP("decorators", init_tests);

	/* Test suite */
	TEST_RUN("unsigned", test_unsigned);
	TEST_RUN("int", test_int);
	TEST_RUN("float", test_float);
	TEST_RUN("bin", test_bin);
	TEST_RUN("oct", test_oct);
	TEST_RUN("hex", test_hex);

	/* Clean up */
	TEST_TEARDOWN(destroy_tests);
}