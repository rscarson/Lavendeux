#include <stdlib.h>

#include "parse.h"
#include "list.h"

/**
 * Allocate a new list
 * @param lst List structure pointer
 * @param size Initial list capacity
 *
 * @return Result
 */
int list_create(list* lst, unsigned int size) {
	lst->capacity = size;
	lst->size = 0;

	lst->elements = (value*) malloc(sizeof(value) * lst->capacity);
		if (lst->elements == NULL)
			return FAILURE_ALLOCATION;

	return NO_FAILURE;
}

/**
 * Add an item to a list
 * @param lst List structure pointer
 * @param e Value to add
 *
 * @return Result
 */
int list_add(list* lst, value e) {
	if (lst->size == lst->capacity) {
		lst->capacity *= 2;
		lst->elements = (value*) realloc(lst->elements, sizeof(value) * lst->capacity);
		if (lst->elements == NULL)
			return FAILURE_ALLOCATION;
	}

	lst->elements[lst->size++] = e;
	return NO_FAILURE;
}

/**
 * Destroy a list
 * @param lst List structure pointer
 */
void list_destroy(const list* lst) {
	free(lst->elements);
}