#include <stdlib.h>
#include <stdio.h>

#include "test.h"
#include "parse.h"
#include "interface.h"

int init_tests( void ) {
	ASSERT_EQUAL(NO_FAILURE, parser_init());
	return 0;
}

int destroy_tests( void ) {
	parser_destroy();

	return 0;
}

int test_base_values( void ) {
	value v;

	ASSERT_EQUAL(NO_FAILURE, parse_equation(L"10", &v, SETTING_ANGLE_DEG));
	ASSERT_WSTR_EQUAL(L"10", v.sv);

	ASSERT_EQUAL(NO_FAILURE, parse_equation(L"10.0", &v, SETTING_ANGLE_DEG));
	ASSERT_WSTR_EQUAL(L"10.0", v.sv);
	return 0;
}

int main() {
	/* Set ourselves up */
	TEST_SETUP("parse", init_tests);

	/* Test suite */
	TEST_RUN("base_values", test_base_values);

	/* Clean up */
	TEST_TEARDOWN(destroy_tests);
}