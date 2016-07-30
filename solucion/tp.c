#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "socket.h"
#include "rs_local.h"
#include "rs_remote.h"

#define MODE_POS 1
#define PARAMS_MIN 3
#define PARAMS_CLIENT 8
#define HOSTNAME_POS 2
#define CLIENT_PORT_POS 3
#define BLOCK_SIZE_POS 7
#define OLD_LOCAL_FILE_POS 4
#define NEW_REMOTE_FILE_POS 6
#define NEW_LOCAL_FILE_POS 5
#define PARAMS_SERVER 3
#define SERVER_PORT_POS 2


int main(int argc, char* argv[]) {
	if (argc < PARAMS_MIN) return EXIT_FAILURE;

	if (!strcmp(argv[MODE_POS], "client")) {
		if (argc != PARAMS_CLIENT) return EXIT_FAILURE;
		
		socket_t client;
		rs_local_t local;
		local_init(&local, &client, argv[HOSTNAME_POS], argv[CLIENT_PORT_POS]);
		local_send_sync_data(&local, argv[OLD_LOCAL_FILE_POS], 
			argv[NEW_REMOTE_FILE_POS], atoi(argv[BLOCK_SIZE_POS]));
		local_file_update(&local, argv[NEW_LOCAL_FILE_POS]);
		local_destroy(&local);

	} else if (!strcmp(argv[MODE_POS], "server")) {
		if (argc != PARAMS_SERVER) return EXIT_FAILURE;
		
		socket_t acceptor;
		socket_t server;
		rs_remote_t remote;
		remote_init(&remote, &acceptor, &server, argv[SERVER_PORT_POS]);
		remote_receive_sync_data(&remote);
		remote_file_cmp(&remote);
		remote_destroy(&remote);

	} else {
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
