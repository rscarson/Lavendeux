#ifndef HASHING_H
	#define HASHING_H

	typedef struct node {
		const char* key;
		void* value;

		struct node *next;
	} hash_node;

	typedef struct {
		unsigned int size;
		hash_node **entries;
	} hash_table;

	typedef void (*value_destructor)(void*);

	unsigned long get_hash(const char*);
	
	int table_create(hash_table*, unsigned int);
	void table_destroy(hash_table*, value_destructor);

	int table_put(hash_table*, const char*, void*);
	void* table_get(hash_table*, const char*);
#endif