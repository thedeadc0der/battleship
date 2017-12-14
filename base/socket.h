#ifndef BASE_SOCKET_H
#define BASE_SOCKET_H

#include "common.h"

#include <sys/socket.h>
#include <arpa/inet.h>

#define SOCK_FORCE_IPV4

#define SOCK_ADDRLEN INET6_ADDRSTRLEN

typedef struct {
	int						fd;
	struct sockaddr_storage	addr;
	socklen_t				addr_len;
	const char *			host;
	const char *			port;
} socket_t;

bool SOCK_Server(socket_t *s, const char *serviceOrPort);
bool SOCK_Client(socket_t *s, const char *host, const char *serviceOrPort);
bool SOCK_Accept(const socket_t *server, socket_t *client);
void SOCK_Close(socket_t *s);
void SOCK_AddressAndPort(socket_t *s, char *addr, char *port);

bool SOCK_Send(socket_t *s, const void *src, size_t len);
bool SOCK_Receive(socket_t *s, void *dst, size_t len);

size_t SOCK_SendString(socket_t *s, const char *src);
size_t SOCK_ReceiveString(socket_t *s, char *dst, size_t dstLen);
size_t SOCK_SendLine(socket_t *s, const char *src);
size_t SOCK_ReceiveLine(socket_t *s, char *dst, size_t dstLen);

#endif
