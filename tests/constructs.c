#include <stdlib.h>
#include <stdio.h>

#include "test.h"
#include "constructs.h"
#include "parse.h"

int init_tests( void ) {
	value *v;

	ASSERT_EQUAL(NO_FAILURE, constructs_init());

	v = (value*) malloc(sizeof(value));
	ASSERT_NOT_NULL(v);

	v->type = VALUE_INT;
	ASSERT_EQUAL(NO_FAILURE, put_variable(L"test", v));

	return 0;
}

int destroy_tests( void ) {
	constructs_destroy();

	return 0;
}

int test_exists( void ) {
	value v;

	ASSERT_EQUAL(NO_FAILURE, get_variable(L"test", &v));
	ASSERT_EQUAL(VALUE_INT, v.type);
	return 0;
}

int test_not_exists( void ) {
	value v;

	ASSERT_EQUAL(FAILURE_INVALID_NAME, get_variable(L"test2", &v));
	return 0;
}

int main() {
	/* Set ourselves up */
	TEST_SETUP("constructs", init_tests);

	/* Test suite */
	TEST_RUN("exists", test_exists);
	TEST_RUN("not_exists", test_not_exists);

	/* Clean up */
	TEST_TEARDOWN(destroy_tests);
}