#ifndef BASE_NET_H
#define BASE_NET_H

#include "base/socket.h"

#include <stdio.h>

#define NET_REQUEST_MAX_FIELDS		64
#define NET_REQUEST_FIELD_LENGTH	64

typedef enum {
	// 1xx: account commands
	NET_REQUEST_ACC_LOGIN	= 100,
	NET_REQUEST_ACC_CREATE	= 101,
	
	// 2xx: game commands
	NET_REQUEST_GAME_CREATE	= 200,
	NET_REQUEST_GAME_LIST	= 201,
	NET_REQUEST_GAME_JOIN	= 202,
	NET_REQUEST_GAME_LEAVE	= 203,
	NET_REQUEST_GAME_MOVE	= 204,
} NET_RequestCode;

typedef enum {
	NET_RESPONSE_OK				= 100,
	NET_RESPONSE_BAD_REQUEST	= 200,
	NET_RESPONSE_BAD_PROTOCOL	= 201,
	NET_RESPONSE_NOT_AVAILABLE	= 202,
	NET_RESPONSE_NOT_FOUND		= 203,
	NET_RESPONSE_OUT_OF_BOUNDS	= 204,
} NET_ResponseCode;

typedef struct {
	char name[NET_REQUEST_FIELD_LENGTH];
	char value[NET_REQUEST_FIELD_LENGTH];
} NET_Field;

typedef struct {
	int			code;
	size_t		field_count;
	NET_Field	fields[NET_REQUEST_MAX_FIELDS];
} NET_Request;

void NET_InitRequest(NET_Request *req, int code);
bool NET_RequestHasField(NET_Request *req, const char *name);
char * NET_GetRequestField(NET_Request *req, const char *name);
void NET_SetRequestField(NET_Request *req, const char *name, const char *value);
void NET_DumpRequest(NET_Request *req, FILE *f);

bool NET_SendRequest(socket_t *link, const NET_Request *req);
bool NET_ReceiveRequest(socket_t *link, NET_Request *req);

void NET_SendCode(socket_t *link, NET_ResponseCode err);

// The code for requests and responses is the same anyway
#define NET_Response			NET_Request
#define NET_InitResponse		NET_InitRequest
#define NET_ResponseHasField	NET_RequestHasField
#define NET_GetResponseField	NET_GetRequestField
#define NET_SetResponseField	NET_SetRequestField
#define NET_SendResponse		NET_SendRequest
#define NET_ReceiveResponse		NET_ReceiveRequest

#endif