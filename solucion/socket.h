#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif

#ifndef SOCKET_H
#define SOCKET_H

#include <stddef.h>
#include <netdb.h>

typedef struct _socket {
    int fd;
} socket_t;

typedef enum _socket_error {
  SUCCESS,
  INIT_ERROR,
  CLOSE_ERROR,
  BIND_ERROR,
  LISTEN_ERROR,
  CONNECT_ERROR,
  ACCEPT_ERROR,
  SEND_ERROR,
  RECEIVE_ERROR,
  SHUTDOWN_ERROR
} socket_error_t;

socket_error_t socket_init(socket_t* self, struct addrinfo* info);

socket_error_t socket_destroy(socket_t* self);

socket_error_t socket_bind_and_listen(socket_t* self, struct addrinfo* info);

socket_error_t socket_connect(socket_t* self, struct addrinfo* info);

socket_error_t socket_accept(socket_t* self, socket_t* server);

socket_error_t socket_send(socket_t* self, char* buffer, size_t size);

socket_error_t socket_receive(socket_t* self, char* buffer, size_t size);

socket_error_t socket_shutdown(socket_t* self);

#endif // SOCKET_H
