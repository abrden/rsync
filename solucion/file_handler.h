#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct _file_handler {
	FILE* fp;
	size_t block_size;
} file_handler_t;

bool file_handler_init(file_handler_t* self, char* file_name, 
						char* mode, size_t block_size);

bool file_handler_read(file_handler_t* self, char* block);

bool file_handler_read_nblock(file_handler_t* self, char* block, size_t n);

bool file_handler_write(file_handler_t* self, char* block);

size_t file_handler_size(file_handler_t* self);

bool file_handler_eof(file_handler_t* self);

void file_handler_rewind_one(file_handler_t* self);

void file_handler_destroy(file_handler_t* self);

#endif // FILE_HANDLER_H
