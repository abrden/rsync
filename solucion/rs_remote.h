#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#ifndef RS_REMOTE_H
#define RS_REMOTE_H

#include "socket.h"
#include "list.h"

typedef struct _rs_remote {
	socket_t* acceptor;
	socket_t* server;
	list_t* checksumized_file;
	char* new_remote_file;
	int block_size;
} rs_remote_t;

void remote_init(rs_remote_t* self, socket_t* acceptor, 
					socket_t* server, char* port);

void remote_receive_sync_data(rs_remote_t* self);

void remote_file_cmp(rs_remote_t* self);

void remote_destroy(rs_remote_t* self);

#endif // RS_REMOTE_H
