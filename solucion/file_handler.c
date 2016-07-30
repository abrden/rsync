#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#include "file_handler.h"

#define ACCOUNT_THE_END_OF_STR 1
#define REWIND_ONE 1

bool file_handler_init(file_handler_t* self, char* file_name, 
						char* mode, size_t block_size) {
	self->fp = fopen(file_name, mode);
	if (!self->fp) return false;
	self->block_size = block_size;
	return true;
}

bool file_handler_read(file_handler_t* self, char* block) {
	if (fgets(block, self->block_size + ACCOUNT_THE_END_OF_STR, self->fp))
		return true;
	return false;
}

bool file_handler_read_nblock(file_handler_t* self, char* block, size_t n) {
	fseek(self->fp, n * self->block_size, SEEK_SET);
	return file_handler_read(self, block);
}

bool file_handler_write(file_handler_t* self, char* block) {
	if (fputs(block, self->fp) > 0);
		return true;
	return false;
}

size_t file_handler_size(file_handler_t* self) {
	long pos = ftell(self->fp);
	fseek(self->fp, 0, SEEK_END);
	long size = ftell(self->fp);
	fseek(self->fp, pos, SEEK_SET);
	return size;
}

bool file_handler_eof(file_handler_t* self) {
	return ftell(self->fp) == file_handler_size(self);
}

void file_handler_rewind_one(file_handler_t* self) {
	fseek(self->fp, - self->block_size + REWIND_ONE, SEEK_CUR);
}

void file_handler_destroy(file_handler_t* self) {
	fclose(self->fp);
}
