#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#include <string.h>
#include <stdlib.h>

#include "rs_remote.h"
#include "socket.h"
#include "list.h"
#include "file_handler.h"
#include "checksum.h"

#define CHECKSUM_END_ID 2
#define NEW_DATA_ID 3
#define DATA_COINCIDENCE_ID 4
#define DATA_END_ID 5
#define BLOCK_NOT_FOUND -1

void remote_init(rs_remote_t* self, socket_t* acceptor, 
					socket_t* server, char* port) {
	struct addrinfo hints;
	struct addrinfo *ptr;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;       /* IPv4 (or AF_INET6 for IPv6)   */
	hints.ai_socktype = SOCK_STREAM; /* TCP  (or SOCK_DGRAM for UDP)  */
	hints.ai_flags = AI_PASSIVE;     /* AI_PASSIVE for server         */
	
	getaddrinfo(NULL, port, &hints, &ptr);
		
	if (socket_init(acceptor, ptr)) {
		freeaddrinfo(ptr);
		return;
	}

	if (socket_bind_and_listen(acceptor, ptr)) {
		freeaddrinfo(ptr);
		return;
	}

	freeaddrinfo(ptr);

	socket_accept(acceptor, server);
	
	self->acceptor = acceptor;
	self->server = server;
}

static char* receive_remote_file(socket_t* server) {
	int file_name_size;
    socket_receive(server, (char*) &file_name_size, sizeof(int));
    
    char* new_remote_file = (char*) malloc(sizeof(char) * (file_name_size + 1));
    
    socket_receive(server, new_remote_file, sizeof(char) * file_name_size);
    new_remote_file[file_name_size] = '\0';
    
    return new_remote_file;
}

static int receive_block_size(socket_t* server) {
	int block_size;
    socket_receive(server, (char*) &block_size, sizeof(int));
    return block_size;
}

static list_t* receive_checksumized_file(socket_t* server) {
	list_t* checksumized_file = list_init();
	char id, end_id, *cs;
	end_id = CHECKSUM_END_ID;
	
	socket_receive(server, &id, sizeof(char));
	
	while (id != end_id) {
		cs = (char*) malloc(sizeof(int));
		socket_receive(server, cs, sizeof(int));
		list_append(checksumized_file, cs);
		socket_receive(server, &id, sizeof(char));
	}
	
	return checksumized_file;
}

void remote_receive_sync_data(rs_remote_t* self) {
	self->new_remote_file = receive_remote_file(self->server);
	self->block_size = receive_block_size(self->server);
	self->checksumized_file = receive_checksumized_file(self->server);
}

static int checksum_is_in(list_t* checksumized_file, int checksum) {
	int* checksum_tmp;
	for (int i = 0; i < list_length(checksumized_file); i++) {
		checksum_tmp = list_get(checksumized_file, i);
		if (checksum == *checksum_tmp) return i;
	}
	return BLOCK_NOT_FOUND;
}

static void send_differences(socket_t* server, list_t* buffer) {
	char new_id, *char_tmp;
	new_id = NEW_DATA_ID;
	int buffer_len = list_length(buffer);
	
	if (buffer_len != 0) {
		socket_send(server, &new_id, sizeof(char));
		socket_send(server, (char*) &buffer_len, sizeof(int));
	}
	
	for (int i = 0; i < buffer_len; i++) {
		char_tmp = list_get(buffer, i);
		socket_send(server, char_tmp, sizeof(char));
	}
}

static void send_coincidence(socket_t* server, int block_num) {
	char coincidence_id = DATA_COINCIDENCE_ID;
	socket_send(server, &coincidence_id, sizeof(char));
	socket_send(server, (char*) &block_num, sizeof(int));
}

static void send_data_end(socket_t* server) {
	char end_id = DATA_END_ID;
	socket_send(server, &end_id, sizeof(char));
}

void remote_file_cmp(rs_remote_t* self) {
	char block[self->block_size], *c;
	int block_num;
	list_t* buffer = list_init();
	file_handler_t fh;
	file_handler_init(&fh, self->new_remote_file, "r", self->block_size);
	
	while (!file_handler_eof(&fh)) {
		file_handler_read(&fh, block);
		block_num = checksum_is_in(self->checksumized_file, 
									checksum(block, self->block_size));
		
		if (block_num == BLOCK_NOT_FOUND) {
			if (file_handler_eof(&fh)) {
				for (int i = 0; i < strlen(block); i++) {
					c = (char*) malloc(sizeof(char));
					*c = block[i];
					list_append(buffer, c);
				}

			} else {
				c = (char*) malloc(sizeof(char));
				*c = block[0];
				list_append(buffer, c);
				file_handler_rewind_one(&fh);
			}
			
		} else {
			send_differences(self->server, buffer);
			list_destroy(buffer, free); 
			buffer = list_init();

			send_coincidence(self->server, block_num);
		}
	}
	
	send_differences(self->server, buffer);
	send_data_end(self->server);
	
	list_destroy(self->checksumized_file, free);
	list_destroy(buffer, free);
	file_handler_destroy(&fh);
}

void remote_destroy(rs_remote_t* self) {
	free(self->new_remote_file);
	socket_shutdown(self->server);
    socket_destroy(self->server);
    socket_shutdown(self->acceptor);
    socket_destroy(self->acceptor);
}
