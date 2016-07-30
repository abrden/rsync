#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#ifndef RS_LOCAL_H
#define RS_LOCAL_H

#include "socket.h"

typedef struct _rs_local {
	socket_t* client;
	char* old_local_file;
	int block_size;
} rs_local_t;

void local_init(rs_local_t* self, socket_t* client, char* hostname, char* port);

void local_send_sync_data(rs_local_t* self, char* local_file_name, 
							char* remote_file_name, int block_size);

void local_file_update(rs_local_t* self, char* new_local_file);

void local_destroy(rs_local_t* self);

#endif // RS_LOCAL_H
