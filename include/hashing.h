#ifndef HASHING_H
	#define HASHING_H

	#include <stdlib.h>

	#define HASH_DEFAULT_SIZE 100

	typedef struct node {
		const wchar_t* key;
		void* value;

		struct node *next;
	} hash_node;

	typedef struct {
		unsigned int size;
		hash_node **entries;
	} hash_table;

	typedef void (*value_destructor)(void*);

	unsigned long get_hash(const wchar_t*);
	
	int table_create(hash_table*, unsigned int);
	void table_destroy(hash_table*, value_destructor);

	int table_put(hash_table*, const wchar_t*, void*, value_destructor);
	void* table_get(hash_table*, const wchar_t*);

	void table_remove(hash_table*, const wchar_t*, value_destructor);
	int table_has(hash_table*, const wchar_t*);
#endif