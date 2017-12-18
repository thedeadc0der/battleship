/* 
 * Anonymous Mode
 * 
 * The client starts in anonymous mode. In stays in that mode until the user
 * eithers logs in or registers. This file implements the user interaction
 * while in anonymous mode.
 */

#include "client.h"
#include "base/cli.h"

#include <stdlib.h>
#include <string.h>

static void do_login(){
	char login[NAME_MAX], pass[NAME_MAX];
	
	// Ask the user for their login info.
	printf("Name: ");
	CLI_ReadLine(login, NAME_MAX);
	printf("Password: ");
	CLI_ReadLine(pass, NAME_MAX);
	
	// Send that to the server
	NET_Request req;
	NET_InitRequest(&req, NET_REQUEST_ACC_LOGIN);
	NET_SetRequestField(&req, "name", login);
	NET_SetRequestField(&req, "pass", pass);
	NET_SendRequest(&CLT_CurrentState.link, &req);
	
	// Receive and read the response
	NET_Response res;
	NET_ReceiveResponse(&CLT_CurrentState.link, &res);
	
	if( res.code == NET_RESPONSE_OK ){
		// If it worked, send the user to the list of games.
		CLT_CurrentState.state = CLT_STATE_GAME_LIST;
		strcpy(CLT_CurrentState.name, login);
	} else {
		// If it didn't work, tell them why.
		fprintf(stderr, "CLT: Login error (response code %d)\n", res.code);
	}
}

static void do_register(){
	char login[NAME_MAX], pass[NAME_MAX];
	
	// Ask the user for their login info.
	puts("Enter the name and password that you want to use");
	printf("Name: ");
	CLI_ReadLine(login, NAME_MAX);
	printf("Password: ");
	CLI_ReadLine(pass, NAME_MAX);
	
	printf("CLT: Registering user (%s) with pass (%s)\n", login, pass);
	// Send that to the server
	NET_Request req;
	NET_InitRequest(&req, NET_REQUEST_ACC_CREATE);
	NET_SetRequestField(&req, "name", login);
	NET_SetRequestField(&req, "pass", pass);
	NET_SendRequest(&CLT_CurrentState.link, &req);
	
	// Receive and read the response
	NET_Response res;
	NET_ReceiveResponse(&CLT_CurrentState.link, &res);
	
	if( res.code == NET_RESPONSE_OK ){
		// If it worked, send the user to the list of games.
		CLT_CurrentState.state = CLT_STATE_GAME_LIST;
		strcpy(CLT_CurrentState.name, login);
	} else {
		// If it didn't work, tell them why.
		printf("CLT: Registration error (code %d): ", res.code);
		
		switch(res.code){
			case NET_RESPONSE_BAD_REQUEST:
				puts("Names and passwords may only contain letters, numbers, spaces, dashes and underscores");
				break;
			
			case NET_RESPONSE_NOT_AVAILABLE:
				puts("This name is already taken by another account");
				break;
				
			case NET_RESPONSE_OUT_OF_BOUNDS:
				puts("The server can't store any more accounts");
				break;
			
			default:
				puts("Unknown error");
				break;
		}
	}
}

bool CLT_HandleAnonymousPhase(){
	while( CLT_CurrentState.state == CLT_STATE_ANONYMOUS ){
		const CLI_MenuOption menu[] = {
			{ .value = 1, .title = "Connexion" },
			{ .value = 2, .title = "Inscription" },
			{ .value = 3, .title = "Quitter" },
		};
		
		switch( CLI_Menu("Bienvenue!", menu, 3) ){
			case 3:
				return false;
				
			case 1:
				do_login();
				break;
				
			case 2:
				do_register();
				break;
		}
	}
	
	return true;
}

