#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

typedef struct _list {
	size_t size;
	size_t length;
	void** data;
} list_t;

list_t* list_init();

void list_destroy(list_t *list, void destroy_data(void* data));

void* list_get(list_t* list, size_t i);

bool list_append(list_t* list, void* value);

size_t list_length(list_t* list);

#endif // LIST_H
