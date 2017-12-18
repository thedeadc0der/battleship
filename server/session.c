
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
		
		// During the anonymous phase, only login and create are valid.
		if( req.code != NET_REQUEST_ACC_LOGIN && req.code != NET_REQUEST_ACC_CREATE ){
			NET_SendCode(&session->socket, NET_RESPONSE_BAD_PROTOCOL);
			return false;
		}
		
		// Retrieve the fields, and make sure they're present.
		const char *name, *pass;
		name = NET_GetRequestField(&req, "name");
		pass = NET_GetRequestField(&req, "pass");
		
		if( name == NULL || pass == NULL ){
			NET_SendCode(&session->socket, NET_RESPONSE_BAD_REQUEST);
			return false;
		}
		
		if( req.code == NET_REQUEST_ACC_LOGIN ){
			printf("SRV: Logging in user: name=(%s) password=(%s)\n", name, pass);
			
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
			printf("SRV: Registering user: name=(%s) password=(%s)\n", name, pass);
			
			// Perform the registration.
			SRV_User_Result result;
			SRV_User *user = SRV_RegisterUser(name, pass, &result);
			
			// If there was an error, report it.
			if( user == NULL ){
				NET_ResponseCode res;
				switch(result){
					default:
					case SRV_AddUser_BadName:
					case SRV_AddUser_BadPass:
						res = NET_RESPONSE_BAD_REQUEST;
						break;
					
					case SRV_AddUser_NameTaken:
						res = NET_RESPONSE_NOT_AVAILABLE;
						break;
					
					case SRV_AddUser_MaxCapacity:
						res = NET_RESPONSE_OUT_OF_BOUNDS;
						break;
				}
				
				NET_SendCode(&session->socket, res);
				return false;
			}
			
			// If not, let them become the new user.
			session->user = user;
			NET_SendCode(&session->socket, NET_RESPONSE_OK);
			return true;
		}
	}
}

void * SRV_HandleClient(socket_t *socket){
	// Initialize the session
	SRV_Session session;
	session.socket	= *socket;
	session.user	= NULL;
	
	// The anonymous phase starts when the user connects, and ends as soon as
	// the client makes a successful registration or login request.
	if( !SRV_HandleAnonymousPhase(&session) ){
		SRV_CloseSession(&session);
		return NULL;
	}
	
	return NULL;
}
