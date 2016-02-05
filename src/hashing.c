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

	while (c = *mb_str++)
		hash = ((hash << 5) + hash) + c;
	return hash;
}

/**
 * Create a hash table
 * @param table Pointer to an empty hash_table structure
 * @param size Hash table size.
 *
 * @return 1 on success, 0 otherwise
 */
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

/**
 * Free a hash table
 * @param table Pointer to the hash_table
 * @param destructor Function able to free whatever it is the table contains, or NULL
 */
void table_destroy(hash_table *table, value_destructor destructor) {
	int i;
	hash_node *entry;

	for (i=0; i<table->size; ++i) {
		entry = table->entries[i];
		while (entry != NULL) {
			if (destructor != NULL)
				destructor(entry->value);
			free(entry);
			entry = entry->next;
		}
	}

	free(table->entries);
}

/**
 * Add to a hash table
 * @param table Pointer to the hash_table
 * @param key entry key
 * @param value Pointer to thing we will store
 *
 * @return 1 on success, 0 otherwise
 */
int table_put(hash_table *table, const wchar_t *key, void *value) {
	hash_node *entry;
	hash_node *new_entry;
	unsigned int hash;

	hash = get_hash(key) % table->size;
	entry = table->entries[hash];

	while (entry != NULL) {
		if (wcscmp(entry->key, key) == 0) {
			entry->value = value;
			return 1;
		}

		if (entry->next != NULL)
			entry = entry->next;
		else break;
	}

	new_entry = (hash_node*) malloc(sizeof(hash_node));
	if (new_entry == NULL)
		return 0; /* Failed to allocate node */

	new_entry->key = key;
	new_entry->value = value;
	new_entry->next = NULL;

	if (entry == NULL) {
		table->entries[hash] = new_entry;
	} else {
		entry->next = new_entry;
	}

	return 1;
}

/**
 * Retrieve a value from a hash table
 * @param table Pointer to the hash_table
 * @param key entry key
 *
 * @return A pointer to the thing, or NULL if not found
 */
void* table_get(hash_table *table, const wchar_t *key) {
	hash_node *entry;
	unsigned int hash = get_hash(key) % table->size;
	entry = table->entries[hash];

	while (entry != NULL) {
		if (wcscmp(entry->key, key) == 0)
			return entry->value;
		entry = entry->next;
	}

	return NULL;
}

/**
 * Delete a hash table entry
 * @param table Pointer to the hash_table
 * @param key entry key
 * @param destructor Function able to free whatever it is the table contains, or NULL
 */
void table_remove(hash_table *table, const wchar_t *key, value_destructor destructor) {
	hash_node *entry;
	hash_node *prev;
	unsigned int hash = get_hash(key) % table->size;

	entry = table->entries[hash];
	prev = NULL;

	while (entry != NULL) {
		if (wcscmp(entry->key, key) == 0) {
			if (prev != NULL)
				prev->next = entry->next;
			
			if (destructor != NULL)
				destructor(entry->value);
			free(entry);
			break;
		}

		prev = entry;
		entry = entry->next;
	}
}

/**
 * Query for a value
 * @param table Pointer to the hash_table
 * @param key entry key
 *
 * @return 1 if found, 0 if not
 */
int table_has(hash_table *table, const wchar_t *key) {
	return (table_get(table, key) != NULL);
}