#ifndef BATTLESHIP_SERVER_H
#define BATTLESHIP_SERVER_H

#include <time.h>
#include <stddef.h>
#include <stdbool.h>

#define NAME_MAX 64

void SRV_Init();
void SRV_Finalize();

typedef uint32_t SRV_UserID;

typedef enum {
	SRV_US_Disconnected,
	SRV_US_Hidden,
	SRV_US_Connected,
} SRV_UserStatus;

typedef struct {
	SRV_UserID	id;
	char 		name[NAME_MAX];
	char 		pass[NAME_MAX];
	time_t 		lastLogin;
} SRV_UserInfo;

SRV_Error SRV_Register(const char *login, const char *pass, size_t board_size);
SRV_UserInfo * SRV_Find(const char *login, const char *pass);



void SRV_RegisterUser(const SRV_UserInfo *info);
SRV_UserInfo * SRV_FindUserWithCreds(const char *name, const char *pass);


#endif
