
#include "base/cmdline.h"
#include "base/socket.h"
#include "base/net.h"

#include "server.h"

#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct {
	bool		shouldStop;
	socket_t	socket;
} g_serverState;

void handle_signal(int sig, void(*handler)(int)){
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = SA_RESTART;
	sigaction(sig, &action, NULL);
}

void SRV_Init(){
	SRV_LoadUsers();
}

void SRV_Finalize(){
	SRV_SaveUsers();
}

static void signal_handler(int signal){
	if( signal == SIGINT ){
		g_serverState.shouldStop = false;
		SOCK_Close(&g_serverState.socket);
	}
}

int main(int argc, const char **argv){
	handle_signal(SIGINT, signal_handler);
	
	CMD_Set(argc, argv);
	SRV_Init();
	
	// Start the server
	const char *port = CMD_Option("port", "12345");
	
	if( !SOCK_Server(&g_serverState.socket, port) ){
		fprintf(stderr, "Impossible d'établir un serveur sur le port: %s\n", port);
		return EXIT_FAILURE;
	}
	
	// Accept clients in new threads
	g_serverState.shouldStop = true;
	while(g_serverState.shouldStop){
		socket_t client;
		
		// Accept a new client. If it fails, just ignore it.
		if( !SOCK_Accept(&g_serverState.socket, &client) )
			continue;
		
		// Create a new thread to handle that.
		pthread_t client_thread;
		
		if( pthread_create(&client_thread, NULL, (void*) SRV_HandleClient, (void*) &client) ){
			fprintf(stderr, "Impossible de démarrer un thread\n");
			continue;
		}
		
		// Print a trace of that.
		pthread_detach(client_thread);
		char addr_str[SOCK_ADDRLEN], port_str[SOCK_ADDRLEN];
		SOCK_AddressAndPort(&client, addr_str, port_str);
		printf("SRV: New client on %s:%s\n", addr_str, port_str);
	}
	
	printf("\nSRV: Exiting...\n");
	SRV_Finalize();
	return EXIT_SUCCESS;
}
