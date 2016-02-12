#ifndef LIST_H
	#define LIST_H

	#define DEFAULT_LIST_CAPACITY 10

	typedef struct {
		int capacity;
		int size;

		value *elements;
	} list;

	int list_create(list* lst, unsigned int size);
	int list_add(list* lst, value e);
	void list_destroy(list* lst);
#endif