
#include "base/net.h"

#include <string.h>
#include <stdio.h>

// ============================================================================
// NET_Request
// 
// NET_Request represents a request that can be sent or received.
// NOTE: The exact same code does responses too!
// ============================================================================
void NET_InitRequest(NET_Request *req, int code){
	assert(req != NULL && code >= 0);
	
	memset(req, 0, sizeof(*req));
	req->code = code;
}

// TODO: Use safe string ops
static NET_Field * NET_RequestField(NET_Request *req, const char *name){
	assert(req != NULL && name != NULL && *name != 0);
	
	for(size_t i=0; i < req->field_count; ++i){
		if( strcmp(name, req->fields[i].name) == 0 )
			return &req->fields[i];
	}
	
	return NULL;
}

bool NET_RequestHasField(NET_Request *req, const char *name){
	return NET_RequestField(req, name) != NULL;
}

char * NET_GetRequestField(NET_Request *req, const char *name){
	NET_Field *field = NET_RequestField(req, name);
	
	if( field != NULL )
		return field->value;
	
	return NULL;
}

// TODO: Use safe string ops
void NET_SetRequestField(NET_Request *req, const char *name, const char *value){
	// First try to find an existing field
	NET_Field *field = NET_RequestField(req, name);
	
	// If it doesn't exist, assign the last field to it.
	if( field == NULL ){
		assert(req->field_count < NET_REQUEST_MAX_FIELDS );
		field = &req->fields[req->field_count++];
		strcpy(field->name, name);
	}
	
	// Copy the new value into the field.
	strcpy(field->value, value);
}

#define REQUEST_BUFFER_SIZE (NET_REQUEST_MAX_FIELDS * 2 + 2)
bool NET_SendRequest(socket_t *socket, const NET_Request *req){
	char buffer[REQUEST_BUFFER_SIZE];
	
	// First, send the code
	sprintf(buffer, "%d", req->code);
	
	if( !SOCK_SendLine(socket, buffer) )
		return false;
	
	// Then, send each field
	for(size_t i=0; i < req->field_count; ++i){
		sprintf(buffer, "%s=%s", req->fields[i].name, req->fields[i].value);
		
		if( !SOCK_SendLine(socket, buffer) )
			return false;
	}
	
	// Once we're done, send the end specifier
	if( !SOCK_SendLine(socket, "END") )
		return false;
	
	return true;
}

// TODO: Automatically reply with NET_RESPONSE_BAD_REQUEST when appropriate
bool NET_ReceiveRequest(socket_t *socket, NET_Request *req){
	char buffer[REQUEST_BUFFER_SIZE];
	
	// The first line should contain the code
	if( SOCK_ReceiveLine(socket, buffer, REQUEST_BUFFER_SIZE) == 0 ){
		fprintf(stderr, "[NET] failed to receive initial line\n");
		return false;
	}
	
	sscanf(buffer, "%d", &req->code);
	
	// Which should be followed by a number of fields
	while(1){
		// Receive one line
		if( SOCK_ReceiveLine(socket, buffer, REQUEST_BUFFER_SIZE) == 0 ){
			fprintf(stderr, "[NET] failed to receive field line\n");
			return false;
		}
		
		// If it's an END line, we're done.
		if( strcmp(buffer, "END") == 0 )
			break;
		
		// Split the field into 'name=value'
		char *separator = strchr(buffer, '=');
		
		if( separator == NULL ){
			fprintf(stderr, "[NET] bad line (no separator): %s\n", buffer);
			return false;
		}
		
		// Replace the equals sign with a null terminator so we get 2 strings.
		*separator++ = '\0';
		
		// Record the field into the request.
		NET_SetRequestField(req, buffer, separator);
	}
	
	// We're done here!
	return true;
}

void NET_SendCode(socket_t *link, NET_ResponseCode err){
	NET_Response res;
	NET_InitResponse(&res, err);
	NET_SendResponse(link, &res);
}

void NET_DumpRequest(NET_Request *req, FILE *f){
	printf("+------------------------------------------------------------\n");
	printf("| Code: %d\n", req->code);
	printf("| Fields (%zu fields total)\n", req->field_count);
	
	for(size_t i=0; i < req->field_count; ++i)
		printf("|     [%zu] %s = %s\n", i, req->fields[i].name, req->fields[i].value);
	
	printf("+------------------------------------------------------------\n");
}

