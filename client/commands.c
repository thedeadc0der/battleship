// 
// This file implements every command the user can enter in the client's
// console. Each command has an "availability mask" which is just a bitmask of
// which states the command can be used in.

#include "client.h"

#include "base/net.h"

// ----------------------------------------------------------------------------
// About: show the name and credits of the program
// ----------------------------------------------------------------------------
static void CMD_About(int argc, const char **argv){
	puts("Battleship: a simple networked battleship game.");
	puts("Authors: Jessy VANDERAUGSTRAETE, Alexandre BEN TICHA");
}

// ----------------------------------------------------------------------------
// Login: Attempt to 
// ----------------------------------------------------------------------------
static void CMD_Login(int argc, const char **argv){
	// Make sure they gave a name
	if( argc != 2 ){
		fprintf(stderr, "syntax: login nickname\n");
		return;
	}
	
	// Request their password 
	char pass[1024];
	
	printf("password ");
	fgets(pass, sizeof(pass), stdin);
	
	if( strlen(pass) == 0 ){
		fprintf(stderr, "empty password is invalid\n");
		return;
	}
	
	// Send the login request
	NET_Request req;
	NET_InitRequest(&req, NET_REQUEST_ACC_LOGIN);
	NET_SetRequestField("login", argv[1]);
	NET_SetRequestField("pass", pass);
	NET_SendRequest(CLT_CurrentState.link, &req);
	
	NET_Response res;
	NET_ReceiveResponse(CLT_CurrentState.link, &res);
	
	
}




void CLT_RegisterCommands(){
	CLT_AddCommand("about", CMD_About, CLT_STATE_ANY);
}

