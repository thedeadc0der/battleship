#ifndef BATTLESHIP_SERVER_H
#define BATTLESHIP_SERVER_H

#include "base/net.h"

// Client management
void SRV_HandleClient(socket_t *socket);

// User management
typedef struct SRV_User SRV_User;

typedef enum {
	SRV_AddUser_BadName,
	SRV_AddUser_BadPass,
	SRV_AddUser_NameTaken,
	SRV_AddUser_MaxCapacity,
} SRV_User_Result;

SRV_User *SRV_RegisterUser(const char *name, const char *pass, SRV_User_Result *r);
SRV_User *SRV_FindUserByName(const char *name);
SRV_User *SRV_FindUserByNameAndPass(const char *name, const char *pass);

const char *SRV_UserName(const SRV_User*);
const SRV_User *SRV_UserList();
const SRV_User *SRV_NextUser(const SRV_User *user);

void SRV_LoadUsers();
void SRV_SaveUsers();

// Games management


#endif
