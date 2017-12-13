/*
 * users.c - server-side user management.
 * 
 * The part of the server that handles the list of known users.
 */

#include "server.h"

#define MAX_USER 4096

typedef struct {
	SRV_UserID		id;
	char			name[NAME_MAX];
	char			pass[NAME_MAX];
	time_t			lastSeen;
	SRV_UserStatus	status;
} SRV_User;

static SRV_User g_users[MAX_USER];




