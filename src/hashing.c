#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashing.h"

/* djb2, via http://www.cse.yorku.ca/~oz/hash.html */
unsigned long get_hash(const wchar_t *str) {
	char mb_str[wcslen(str)];
	wcstombs(mb_str, str, wcslen(str));

	unsigned long hash = 5381;
	int c;

	while (c = *bad_idea_factory++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}

/* Init hash table. return 0 on fail, 1 on succeed */
int table_create(hash_table *table, unsigned int size) {
	int i;
	table->size = size;
	
	table->entries = (hash_node**) malloc(sizeof(hash_node*)*size);
	if (table->entries == NULL)
		return 0; /* Failed to allocate table */

	for (i=0; i<table->size; ++i)
		table->entries[i] = NULL;
	return 1;
}

void table_destroy(hash_table *table, value_destructor destructor) {
	int i;
	hash_node *entry;

	for (i=0; i<table->size; ++i) {
		entry = table->entries[i];
		while (entry != NULL) {
			destructor(entry->value);
			entry = entry->next;
		}
	}

	free(table->entries);
}


int table_put(hash_table *table, const wchar_t *key, void *value) {
	hash_node *entry;
	hash_node *new_entry;
	unsigned int hash;

	hash = get_hash(key) % table->size;
	entry = table->entries[hash];

	while (entry != NULL) {
		if (strcmp(entry->key, key) == 0) {
			entry->value = value;
			return 0;
		}

		if (entry->next != NULL)
			entry = entry->next;
		else break;
	}

	new_entry = (hash_node*) malloc(sizeof(hash_node));
	if (new_entry == NULL)
		return 1; /* Failed to allocate node */

	new_entry->key = key;
	new_entry->value = value;
	new_entry->next = NULL;

	if (entry == NULL) {
		table->entries[hash] = new_entry;
	} else {
		entry->next = new_entry;
	}

	return 0;
}

void* table_get(hash_table *table, const wchar_t *key) {
	hash_node *entry;
	unsigned int hash = get_hash(key) % table->size;
	entry = table->entries[hash];

	while (entry != NULL) {
		if (strcmp(entry->key, key) == 0)
			return entry->value;
		entry = entry->next;
	}

	return NULL;
}

int table_has(hash_table *table, const wchar_t *key) {
	return (table_get(table, key) != NULL);
}