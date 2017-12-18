#ifndef BATTLESHIP_CLIENT_H
#define BATTLESHIP_CLIENT_H

#include "base/net.h"

#define NAME_MAX 1024

// Client state
typedef enum {
	CLT_STATE_ANY			= 0xFF,
	
	CLT_STATE_QUIT			= 0x00,
	CLT_STATE_ANONYMOUS		= 0x01,
	CLT_STATE_GAME_LIST		= 0x02,
	CLT_STATE_MY_TURN		= 0x04,
	CLT_STATE_WAITING		= 0x08,
} CLT_StateCode;

typedef struct {
	CLT_StateCode			state;
	socket_t				link;
	char					name[NAME_MAX];
} CLT_State;

extern CLT_State CLT_CurrentState;

bool CLT_HandleAnonymousPhase();

#endif
