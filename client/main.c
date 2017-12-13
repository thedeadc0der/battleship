
#include "base/cmdline.h"
#include "base/socket.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(int argc, const char **argv){
	CMD_Set(argc, argv);
	
	const char *server = CMD_Option("host", "localhost");
	const char *port = CMD_Option("port", "12345");
	
	socket_t client;
	
	if( !SOCK_Client(&client, server, port) ){
		fprintf(stderr, "Impossible de se connecter à %s:%s\n", server, port);
		return EXIT_FAILURE;
	}
	
	if( !SOCK_SendString(&client, "Big Smoke") )
		puts("SOCK_Send failed");
	
	SOCK_Close(&client);
	return 0;
}

