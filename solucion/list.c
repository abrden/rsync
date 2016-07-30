#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include "list.h"

#define INITIAL_SIZE 10
#define RESIZE_FACTOR 2

list_t* list_init() {
	list_t* list = malloc(sizeof(list_t));
	if (!list) return NULL;

	list->data = malloc(INITIAL_SIZE * sizeof(void*));
	if (!list->data) {
	    free(list);
		return NULL;
	}
	list->size = INITIAL_SIZE;
	list->length = 0;
	return list;
}

void list_destroy(list_t *list, void destroy_data(void* data)) {
	if (list) {
		if (destroy_data) {
			for (int i = 0; i < list->length; i++)
				destroy_data(list->data[i]);
		}
		free(list->data);
		free(list);
	}
}

static bool list_resize(list_t* list, size_t new_size) {
	if (!list) return false;
	
	void** new_data = realloc(list->data, new_size * sizeof(void*));
	
	if (new_size > 0 && !new_data) return false;
	
	list->data = new_data;
	list->size = new_size;
	return true;
}

void* list_get(list_t* list, size_t i) {
	if (!list) return NULL;
	if (i >= list->size) return NULL;

	return list->data[i];
}

bool list_append(list_t* list, void* value) {
	if (!list) return false;
	
	if (list->length == list->size)
		if (!list_resize(list, list->size * RESIZE_FACTOR))
			return false;
	
	list->data[list->length] = value;
	list->length++;
	return true;
}

size_t list_length(list_t* list) {
	if (!list) return 0;
	return list->length;
}
