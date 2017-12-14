
#include "server.h"

#include "base/net.h"

typedef struct {
	socket_t	socket;
	SRV_User *	user;
} SRV_Session;

static void SRV_CloseSession(SRV_Session *session){
	SOCK_Close(&session->socket);
}

static bool SRV_HandleAnonymousPhase(SRV_Session *session){
	for(;;){
		// Receive a request.
		NET_Request req;
		
		if( !NET_ReceiveRequest(&session->socket, &req) )
			return false;
		
		// Handle the request
		if( req.code == NET_REQUEST_ACC_LOGIN ){
			const char *name, *pass;
			name = NET_GetRequestField(&req, "name");
			pass = NET_GetRequestField(&req, "pass");
			
			// Empty name and/or pass are invalid.
			if( name == NULL || pass == NULL ){
				NET_SendCode(&session->socket, NET_RESPONSE_BAD_REQUEST);
				return false;
			}
			
			// Look for them in the users list.
			SRV_User *user = SRV_FindUserByNameAndPass(name, pass);
			
			if( user != NULL ){
				session->user = user;
				NET_SendCode(&session->socket, NET_RESPONSE_OK);
				return true;
			}
			
			// If they weren't, tell them and let them try again.
			if( user == NULL )
				NET_SendCode(&session->socket, NET_RESPONSE_NOT_FOUND);
		} else if( req.code == NET_REQUEST_ACC_CREATE ){
			// Registration request: go
			
		} else {
			NET_SendCode(&session->socket, NET_RESPONSE_BAD_PROTOCOL);
			return false;
		}
	}
}

void SRV_HandleClient(socket_t *socket){
	// Initialize the session
	SRV_Session session;
	session.socket	= *socket;
	session.user	= NULL;
	
	// The anonymous phase starts when the user connects, and ends as soon as
	// the client makes a successful registration or login request.
	if( !SRV_HandleAnonymousPhase(&session) ){
		SRV_CloseSession(&session);
		return;
	}
	
	
}
