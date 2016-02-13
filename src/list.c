#include <stdlib.h>

#include "parse.h"
#include "list.h"

int list_create(list* lst, unsigned int size) {
	lst->capacity = size;
	lst->size = 0;

	lst->elements = (value*) malloc(sizeof(value) * lst->capacity);
		if (lst->elements == NULL)
			return FAILURE_ALLOCATION;

	return NO_FAILURE;
}

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

void list_destroy(const list* lst) {
	free(lst->elements);
}