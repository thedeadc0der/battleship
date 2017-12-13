
#include "base/cmdline.h"
#include "base/socket.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, const char **argv){
	CMD_Set(argc, argv);
	
	const char *port = CMD_Option("port", "12345");
	
	socket_t server;
	
	if( !SOCK_Server(&server, port) ){
		fprintf(stderr, "Impossible d'établir le serveur sur le port %s\n", port);
		return EXIT_FAILURE;
	}
	
	char serverIP[SOCK_ADDRLEN], serverPort[SOCK_ADDRLEN];
	SOCK_AddressAndPort(&server, serverIP, serverPort);
	printf("Listening on %s on port %s\n", serverIP, serverPort);
	
	while(true){
		socket_t client;
		
		if( !SOCK_Accept(&server, &client) ){
			perror("Could not accept connection: ");
			break;
		}
		
		char clientIP[SOCK_ADDRLEN], clientPort[SOCK_ADDRLEN];
		SOCK_AddressAndPort(&client, clientIP, clientPort);
		printf("Client: %s on port %s\n", clientIP, clientPort);
		
		char buffer[4096];
		size_t len = SOCK_ReceiveString(&client, buffer, sizeof(buffer) );
		printf("Received %zu characters: %s\n", len, buffer);
		
		SOCK_Close(&client);
	}
	
	SOCK_Close(&server);
	return EXIT_SUCCESS;
}

