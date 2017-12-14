
#include "base/cmdline.h"
#include "base/socket.h"
#include "base/net.h"

#include "server.h"

#include <pthread.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void SRV_Init(){
	SRV_LoadUsers();
}

void SRV_Finalize(){
	SRV_SaveUsers();
}

/*
static void * SRV_HandleClient(socket_t *arg){
	// Copy the socket info.
	socket_t client = *arg;
	
	// Send shit
	NET_Request req;
	NET_ReceiveRequest(&client, &req);
	NET_DumpRequest(&req, stdout);
	
	// We're done, close the socket.
	SOCK_Close(&client);
	return NULL;
}
*/

int main(int argc, const char **argv){
	CMD_Set(argc, argv);
	SRV_Init();
	
	// Start the server
	const char *port = CMD_Option("port", "12345");
	
	socket_t server;
	
	if( !SOCK_Server(&server, port) ){
		fprintf(stderr, "Impossible d'établir un serveur sur le port: %s\n", port);
		return EXIT_FAILURE;
	}
	
	// Accept clients in new threads
	while(true){
		socket_t client;
		
		// Accept a new client. If it fails, just ignore it.
		if( !SOCK_Accept(&server, &client) )
			continue;
		
		// Create a new thread to handle that.
		pthread_t client_thread;
		
		if( pthread_create(&client_thread, NULL, (void*) SRV_HandleClient, (void*) &client) ){
			fprintf(stderr, "Impossible de démarrer un thread\n");
			continue;
		}
		
		// Print a trace of that.
		char addr_str[SOCK_ADDRLEN], port_str[SOCK_ADDRLEN];
		SOCK_AddressAndPort(&client, addr_str, port_str);
		printf("SRV: New client on %s:%s\n", addr_str, port_str);
	}
	
	SOCK_Close(&server);
	SRV_Finalize();
	return EXIT_SUCCESS;
}
