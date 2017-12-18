
#include "base/socket.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BACKLOG 20

#ifdef SOCK_FORCE_IPV4
#	define ADDRESS_FAMILY AF_INET
#else
#	define ADDRESS_FAMILY AF_UNSPEC
#endif

static void SOCK_EnableAddressReuse(int socket){
	int option_value = 1;
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char *) &option_value, sizeof(int));
}

bool SOCK_Server(socket_t *sock, const char *svc){
	assert(sock != NULL);
	assert(svc != NULL && *svc != 0);
	
	// Fill the hints structure for getaddrinfo()
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = ADDRESS_FAMILY; // We don't care if we get IPv4 or IPv6, just make it work
	hints.ai_socktype = SOCK_STREAM; // We'll use stream sockets
	hints.ai_flags = AI_PASSIVE; // We're a server, we'll bind to the local host
	
	// Get our addresses
	struct addrinfo *result = NULL;
	if( getaddrinfo(NULL, svc, &hints, &result) == -1 ){
		perror("getaddrinfo");
		return false;
	}
	
	// Now, try everything getaddrinfo returned, until one works.
	int serverSocket = -1;
	
	for(struct addrinfo *curr=result; curr != NULL; curr = curr->ai_next){
		// First, create the socket
		serverSocket = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
		
		// Didn't work? Tough luck, just try the next one :)
		if( serverSocket == -1 ){
			perror("socket: ");
			continue;
		}
		
		// Then, try to bind to the address.
		if( bind(serverSocket, curr->ai_addr, curr->ai_addrlen) == -1 ){
			perror("bind: ");
			
			// If bind() fails, we still have the socket open, so close it.
			close(serverSocket);
			serverSocket = -1;
			continue;
		}
		
		// Finally, try to listen on the socket.
		if( listen(serverSocket, BACKLOG) == -1 ){
			perror("listen: ");
			// If listen() fails, we still have the socket open, so close it.
			
			close(serverSocket);
			serverSocket = -1;
			continue;
		}
		
		// Make sure we don't get the "address already in use" error.
		SOCK_EnableAddressReuse(serverSocket);
		
		// We've got one that works! Save that info!
		sock->fd		= serverSocket;
		sock->host		= NULL;
		sock->port		= svc;
		memcpy(&sock->addr, curr->ai_addr, curr->ai_addrlen);
		sock->addr_len	= curr->ai_addrlen;
		freeaddrinfo(result);
		return true;
	}
	
	// We couldn't find a configuration that works, so just report failure.
	// But don't forget to free the addrinfo structure!
	freeaddrinfo(result);
	return false;
}

bool SOCK_Client(socket_t *sock, const char *host, const char *svc){
	assert(sock != NULL);
	assert(host != NULL && *host != 0);
	assert(svc != NULL && *svc != 0);
	
	// Fill the hints structure for getaddrinfo()
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = ADDRESS_FAMILY; // We don't care if we get IPv4 or IPv6, just make it work
	hints.ai_socktype = SOCK_STREAM; // We'll use stream sockets
	
	// Get our addresses
	struct addrinfo *result = NULL;
	if( getaddrinfo(host, svc, &hints, &result) == -1 )
		return false;
	
	// Now, try everything getaddrinfo returned, until one works.
	int clientSocket = -1;
	
	for(struct addrinfo *curr=result; curr != NULL; curr = curr->ai_next){
		// First, create the socket
		clientSocket = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
		
		// Didn't work? Tough luck, just try the next one :)
		if( clientSocket == -1 ){
			perror("socket: ");
			continue;
		}
		
		// Make sure we don't get the "address already in use" error.
		SOCK_EnableAddressReuse(clientSocket);
		
		// Now try to connect
		if( connect(clientSocket, curr->ai_addr, curr->ai_addrlen) == -1 ){
			perror("connect: ");
			close(clientSocket);
			clientSocket = -1;
			continue;
		}
		
		// We've got one that works! Save that info!
		sock->fd		= clientSocket;
		sock->host		= host;
		sock->port		= svc;
		memcpy(&sock->addr, curr->ai_addr, curr->ai_addrlen);
		sock->addr_len	= curr->ai_addrlen;
		freeaddrinfo(result);
		return true;
	}
	
	return false;
}

void SOCK_Close(socket_t *sock){
	assert(sock != NULL);
	assert(sock->fd > 0);
	
	shutdown(sock->fd, SHUT_RDWR);
	close(sock->fd);
	memset(sock, 0, sizeof(*sock));
}

bool SOCK_Accept(const socket_t *server, socket_t *client){
	assert(server != NULL && client != NULL);
	assert(server->fd > 0);
	
	// If it's going to fail, we want to leave *client as is, so use an intermediate
	// address structure to store into.
	struct sockaddr_storage client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	int result = accept(server->fd, (struct sockaddr*) &client_addr, &client_addr_len);
	
	// Handle failure gracefully.
	if( result == -1 ){
		memset(client, 0, sizeof(*client));
		return false;
	}
	
	// It worked! Copy the data to the client socket.
	client->fd = result;
	memcpy(&client->addr, &client_addr, client_addr_len);
	client->addr_len = client_addr_len;
	client->host = NULL;
	client->port = NULL;
	return true;
}

void SOCK_AddressAndPort(socket_t *s, char *dst_addr, char *dst_port){
	assert(s != NULL && dst_addr != NULL && dst_port != NULL);
	
	// Convert the address and port to a string.
	// Retrieving these requires diving into protocol-specific stuff.
	if( s->addr.ss_family == AF_INET ){ // IPv4
		struct sockaddr_in *sa = (struct sockaddr_in*) &s->addr;
		inet_ntop(AF_INET, &sa->sin_addr, dst_addr, SOCK_ADDRLEN);
		
		in_port_t port = ntohs(sa->sin_port);
		sprintf(dst_port, "%d", port);
	} else if( s->addr.ss_family == AF_INET6 ){ // IPv6
		struct sockaddr_in6 *sa = (struct sockaddr_in6*) &s->addr;
		inet_ntop(AF_INET6, &sa->sin6_addr, dst_addr, SOCK_ADDRLEN);
		
		in_port_t port = ntohs(sa->sin6_port);
		sprintf(dst_port, "%d", port);
	} else { // Anything else
		strcpy(dst_port, "???");
	}
}

bool SOCK_Send(socket_t *s, const void *srcPtr, size_t len){
	assert(s != NULL && srcPtr != NULL && len > 0);
	
	// Use a typed pointer so we can do math on it.
	const uint8_t *src = (const uint8_t*) srcPtr;
	
	// Keep sending until we've sent everything.
	while( len > 0 ){
		// Try to send what remains.
		ssize_t result = send(s->fd, src, len, 0);
		
		// Detect errors.
		if( result == -1 ){
			perror("send");
			return false;
		}
		
		// Not technically an error, but often indicates a connection reset,
		// which typically causes the same behavior.
		if( result == 0 ){
			fprintf(stderr, "send: connection aborted\n");
			return false;
		}
		
		// Account for what we did send.
		len -= result;
		src += result;
	}
	
	return true;
}

bool SOCK_Receive(socket_t *s, void *dstPtr, size_t len){
	assert(s != NULL && dstPtr != NULL && len > 0);
	
	uint8_t *dst = (uint8_t*) dstPtr;
	
	while( len > 0 ){
		ssize_t result = recv(s->fd, dst, len, 0);
		
		// Detect errors.
		if( result == -1 ){
			perror("recv");
			return false;
		}
		
		// Not technically an error, but often indicates a connection reset,
		// which typically causes the same behavior.
		if( result == 0 ){
			fprintf(stderr, "recv: connection aborted\n");
			return false;
		}
		
		// Account for what we did send.
		len -= result;
		dst += result;
	}
	
	return true;
}

static size_t SOCK_SendTermString(socket_t *s, const char *src, char term){
	assert(s != NULL && src != NULL);
	size_t len = strlen(src);
	
	if( !SOCK_Send(s, src, len) )
		return 0;
	
	if( !SOCK_Send(s, &term, sizeof(term)) )
		return 0;
	
	return len;
}

size_t SOCK_SendString(socket_t *s, const char *src){
	return SOCK_SendTermString(s, src, '\0');
}

size_t SOCK_SendLine(socket_t *s, const char *src){
	return SOCK_SendTermString(s, src, '\n');
}

static size_t SOCK_ReceiveTermString(socket_t *s, char *dst, size_t dstLen, char term){
	assert(s != NULL && dst != NULL);
	assert(dstLen >= 1);
	
	size_t written = 0;
	char c = 0;
	
	while(written < dstLen - 1){
		// Receive one character.
		if( ! SOCK_Receive(s, &c, sizeof(char)) )
			break;
		
		// A null character ends the string.
		if( c == term )
			break;
		
		// It's just a normal character, add it to the string.
		*dst++ = c;
		++written;
	}
	
	// Terminate the string and return its length.
	*dst = '\0';
	return written;
}

size_t SOCK_ReceiveString(socket_t *s, char *dst, size_t dstLen){
	return SOCK_ReceiveTermString(s, dst, dstLen, '\0');
}

size_t SOCK_ReceiveLine(socket_t *s, char *dst, size_t dstLen){
	return SOCK_ReceiveTermString(s, dst, dstLen, '\n');
}

