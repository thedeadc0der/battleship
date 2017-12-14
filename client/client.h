#ifndef BATTLESHIP_CLIENT_H
#define BATTLESHIP_CLIENT_H

#include "base/net.h"

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
} CLT_State;

extern CLT_State CLT_CurrentState;

// Console
void CLT_RegisterCommands();
void CLT_RunConsole();
typedef void(*CLT_CommandFunc)(int argc, const char **argv);
void CLT_AddCommand(const char *name, CLT_CommandFunc cf, unsigned avail);

#endif