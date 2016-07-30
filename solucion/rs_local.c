#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#include <string.h>

#include "rs_local.h"
#include "socket.h"
#include "file_handler.h"
#include "checksum.h"

#define ACCOUNT_THE_END_OF_STR 1
#define CHECKSUM_ID 1
#define CHECKSUM_END_ID 2
#define NEW_DATA_ID 3
#define DATA_COINCIDENCE_ID 4
#define DATA_END_ID 5

void local_init(rs_local_t* self, socket_t* client, 
				char* hostname, char* port) {
	struct addrinfo hints;
	struct addrinfo *result, *ptr;
	bool connected = false;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;       /* IPv4 (or AF_INET6 for IPv6)    */
	hints.ai_socktype = SOCK_STREAM; /* TCP  (or SOCK_DGRAM for UDP)   */
	hints.ai_flags = 0;              /* None (or AI_PASSIVE for server)*/
	
	getaddrinfo(hostname, port, &hints, &result);
    
	for (ptr = result; ptr != NULL && connected == false; ptr = ptr->ai_next) {
		socket_init(client, ptr);
		if (socket_connect(client, ptr) == 0) connected = true;
	}

	freeaddrinfo(result);
	
	self->client = client;
}

static void send_remote_file(socket_t* client, char* new_remote_file) {
    size_t file_name_size = strlen(new_remote_file);
    socket_send(client, (char*) &file_name_size, sizeof(int));
    socket_send(client, new_remote_file, sizeof(char) * file_name_size);
}

static void send_block_size(rs_local_t* self) {
    socket_send(self->client, (char*) &self->block_size, sizeof(int));
}

static void send_checksum(socket_t* client, int cs) {
	char start_id = CHECKSUM_ID;
	socket_send(client, &start_id, sizeof(char));
	socket_send(client, (char*) &cs, sizeof(int));
}

static void send_checksums_end(socket_t* client) {
	char end_id = CHECKSUM_END_ID;
	socket_send(client, &end_id, sizeof(char));
}

static void send_checksumized_file(rs_local_t* self) {
	int kBlockSize = self->block_size;
	char block[kBlockSize];
	int cs;
	file_handler_t fh;
	file_handler_init(&fh, self->old_local_file, "r", self->block_size);
	
	for (int i = 0; i < (file_handler_size(&fh) / self->block_size); i++) {
		file_handler_read(&fh, block);
		cs = checksum(block, self->block_size);
		send_checksum(self->client, cs);
	}
	
	send_checksums_end(self->client);
	
	file_handler_destroy(&fh);
}

void local_send_sync_data(rs_local_t* self, char* local_file_name, 
							char* remote_file_name, int block_size) {
	self->block_size = block_size;
	self->old_local_file = local_file_name;
	send_remote_file(self->client, remote_file_name);
	send_block_size(self);
	send_checksumized_file(self);
}

static void notify_new_reception(int char_qty) {
	printf("RECV File chunk %d bytes\n", char_qty);
}

static void notify_coincidence_reception(int block_num) {
	printf("RECV Block index %d\n", block_num);
}

static void notify_eof() {
	printf("RECV End of file\n");
}

void local_file_update(rs_local_t* self, char* new_local_file) {
	int block_num, char_qty, kBufferSize, kBlockSize;
	kBlockSize = self->block_size;
	char id, block[kBlockSize];
	
	file_handler_t old_fh;
	file_handler_init(&old_fh, self->old_local_file, "r", self->block_size);
	file_handler_t new_fh;
	file_handler_init(&new_fh, new_local_file, "w", self->block_size);
	
	socket_receive(self->client, &id, sizeof(char));
	while (id != DATA_END_ID) {
		if (id == NEW_DATA_ID) {
			socket_receive(self->client, (char*) &char_qty, sizeof(int));
			notify_new_reception(char_qty);
			kBufferSize = char_qty + 1;
			char buffer[kBufferSize];
			buffer[char_qty] = '\0';
			socket_receive(self->client, buffer, sizeof(char) * char_qty);
			file_handler_write(&new_fh, buffer);
		} else if (id == DATA_COINCIDENCE_ID) {
			socket_receive(self->client, (char*) &block_num, sizeof(int));
			notify_coincidence_reception(block_num);
			file_handler_read_nblock(&old_fh, block, block_num);
			file_handler_write(&new_fh, block);
		}
		socket_receive(self->client, &id, sizeof(char));	
	}
	notify_eof();
	
	file_handler_destroy(&old_fh);
	file_handler_destroy(&new_fh);
}

void local_destroy(rs_local_t* self) {
	socket_shutdown(self->client);
    socket_destroy(self->client);
}
