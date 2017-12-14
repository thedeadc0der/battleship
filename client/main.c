
#include "client.h"

#include "base/cmdline.h"
#include "base/socket.h"
#include "base/net.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

CLT_State CLT_CurrentState = {
	.state = CLT_STATE_ANONYMOUS,
};

int main(int argc, const char **argv){
	CMD_Set(argc, argv);
	
	// Start the client connection.
	const char *server = CMD_Option("host", "localhost");
	const char *port = CMD_Option("port", "12345");
	
	socket_t *link = &CLT_CurrentState.link;
	
	if( !SOCK_Client(link, server, port) ){
		fprintf(stderr, "Impossible de se connecter à %s:%s\n", server, port);
		return EXIT_FAILURE;
	}
	
	CLT_RegisterCommands();
	
	// Let the user log in or register
	while( CLT_CurrentState.state == CLT_STATE_ANONYMOUS )
		CLT_RunConsole();
	
	SOCK_Close(link);
	return 0;
}

