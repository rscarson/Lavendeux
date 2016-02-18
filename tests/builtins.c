#include <stdlib.h>
#include <stdio.h>

#include "test.h"
#include "builtins.h"
#include "constructs.h"
#include "parse.h"

int init_tests( void ) {
	ASSERT_EQUAL(NO_FAILURE, builtins_init());

	return 0;
}

int destroy_tests( void ) {
	builtins_destroy();

	return 0;
}

int test_is_builtin( void ) {
	ASSERT_TRUE(is_builtin(L"floor"));
	return 0;
}

int main() {
	/* Set ourselves up */
	TEST_SETUP("builtins", init_tests);

	/* Test suite */
	TEST_RUN("is_builtin", test_is_builtin);

	/* Clean up */
	TEST_TEARDOWN(destroy_tests);
}