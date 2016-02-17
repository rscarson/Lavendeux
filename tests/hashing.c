#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "test.h"
#include "hashing.h"

int* value;
hash_table test_table;

int init_tests( void ) {
	ASSERT_TRUE(table_create(&test_table, HASH_DEFAULT_SIZE));

	value = (int*) malloc(sizeof(int));
	ASSERT_NOT_NULL(value);
	*value = 0;

	ASSERT_TRUE(table_put(&test_table, L"test", value, NULL));
	return 0;
}

int destroy_tests( void ) {
	table_destroy(&test_table, NULL);

	return 0;
}

int test_get( void ) {
	ASSERT_EQUAL(value, table_get(&test_table, L"test"));
	return 0;
}

int test_has( void ) {
	ASSERT_TRUE(table_has(&test_table, L"test"));
	return 0;
}

int test_hasnt( void ) {
	ASSERT_FALSE(table_has(&test_table, L"test2"));
	return 0;
}

int test_remove( void ) {
	ASSERT_TRUE(table_put(&test_table, L"test2", value, NULL));
	ASSERT_TRUE(table_has(&test_table, L"test2"));

	table_remove(&test_table, L"test2", NULL);
	ASSERT_FALSE(table_has(&test_table, L"test2"));
	return 0;
}

int main() {
	/* Set ourselves up */
	TEST_SETUP("hashing", init_tests);

	/* Test suite */
	TEST_RUN("get", test_get);
	TEST_RUN("has", test_has);
	TEST_RUN("hasnt", test_hasnt);
	TEST_RUN("remove", test_remove);

	/* Clean up */
	TEST_TEARDOWN(destroy_tests);
}