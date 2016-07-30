#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>

#include "socket.h"

#define LISTEN_LIMIT 10
#define ERROR_CODE -1

socket_error_t socket_init(socket_t* self, struct addrinfo* info) {
	self->fd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);
	if (self->fd == ERROR_CODE)
		return INIT_ERROR;
	return SUCCESS;
}

socket_error_t socket_destroy(socket_t* self) {
	if (close(self->fd) == ERROR_CODE)
		return CLOSE_ERROR;
	return SUCCESS;
}

socket_error_t socket_bind_and_listen(socket_t* self, struct addrinfo* info) {
	if (bind(self->fd, info->ai_addr, info->ai_addrlen) == ERROR_CODE)
		return BIND_ERROR;

	if (listen(self->fd, LISTEN_LIMIT) == ERROR_CODE)
		return LISTEN_ERROR;

	return SUCCESS;
}

socket_error_t socket_connect(socket_t* self, struct addrinfo* info) {
	if (connect(self->fd, info->ai_addr, info->ai_addrlen) == ERROR_CODE)
		return CONNECT_ERROR;
	return SUCCESS;
}

socket_error_t socket_accept(socket_t* self, socket_t* server) {
	server->fd = accept(self->fd, NULL, NULL);
	if (server->fd == ERROR_CODE)
		return ACCEPT_ERROR;
	
	return SUCCESS;
}

socket_error_t socket_send(socket_t* self, char* buffer, size_t size) {
	int sent, sent_now;
	sent = sent_now  = 0;
	bool valid_socket = true;

	while (sent < size && valid_socket) {
		sent_now = send(self->fd, &buffer[sent], size - sent, MSG_NOSIGNAL);
		
		if (!sent_now || sent_now == ERROR_CODE)
			valid_socket = false;
		else
			sent += sent_now;
	}
	
	if (valid_socket) return SUCCESS;
	return SEND_ERROR;
}

socket_error_t socket_receive(socket_t* self, char* buffer, size_t size) {
	int received, received_now;
	received = received_now = 0;
	bool valid_socket = true;

	while (received < size && valid_socket) {
		received_now = recv(self->fd, &buffer[received], 
							size - received, MSG_NOSIGNAL);
      
		if (!received_now || received_now == ERROR_CODE)
			valid_socket = false;
		else
			received += received_now;
	}

	if (valid_socket) return SUCCESS;
	return RECEIVE_ERROR;
}

socket_error_t socket_shutdown(socket_t* self) {
	if (shutdown(self->fd, SHUT_RDWR) == ERROR_CODE)
		return SHUTDOWN_ERROR;
	return SUCCESS;
}
