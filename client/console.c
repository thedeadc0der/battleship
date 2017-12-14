
#include "client.h"

#include <string.h>
#include <ctype.h>

#define COMMAND_MAX			32
#define COMMAND_BUFFER_MAX	1024
#define COMMAND_COUNT		1024
#define ARGC_MAX			16

// ============================================================================
// Command list management
// ============================================================================
typedef struct {
	char				name[COMMAND_MAX];
	CLT_CommandFunc		command_fn;
	unsigned			avail_states;
} CLT_Command;

static CLT_Command g_commands[COMMAND_COUNT];
static size_t g_command_cnt = 0;

void CLT_AddCommand(const char *name, CLT_CommandFunc cf, unsigned avail){
	assert(g_command_cnt < COMMAND_COUNT);
	
	CLT_Command cmd;
	strcpy(cmd.name, name);
	cmd.command_fn = cf;
	cmd.avail_states = avail;
	
	memcpy(&g_commands[g_command_cnt++], &cmd, sizeof(cmd));
}

CLT_Command *CLT_FindCommand(const char *name){
	for(size_t i=0; i < g_command_cnt; ++i){
		if( strcmp(name, g_commands[i].name) == 0 )
			return &g_commands[i];
	}
	
	return NULL;
}

// ============================================================================
// Console management
// ============================================================================
static void read_line(char *dst, size_t limit){
	size_t pos = 0;
	
	while( pos < limit - 1 ){
		char curr = fgetc(stdin);
		
		if( curr == '\n' )
			break;
		
		dst[pos++] = curr;
	}
	
	dst[pos] = '\0';
	fflush(stdin);
}

static void split_command(char *cmd, int *argc, char **argv){
	int curr_argc = 0;
	
	while(curr_argc < ARGC_MAX){
		while( *cmd == ' ' && *cmd != '\0' )
			cmd++;
		
		if( *cmd == '\0' )
			break;
		
		char *next_limit = strchr(cmd, ' ');
		
		if( next_limit == NULL ){
			// We reached the last word
			argv[curr_argc++] = cmd;
			break;
		} else {
			// There is a word after this one
			argv[curr_argc++] = cmd;
			*next_limit = '\0';
			cmd = next_limit + 1;
		}
	}
	
	*argc = curr_argc;
}

void CLT_RunConsole(){
	// Give a custom prompt
	printf("battleship> ");
	
	// Read the input command
	char buffer[COMMAND_BUFFER_MAX];
	read_line(buffer, COMMAND_BUFFER_MAX);
	
	// Split it into argc/argv
	int argc; char *argv[ARGC_MAX];
	split_command(buffer, &argc, argv);
	
	if( argc > 0 ){
		CLT_Command *cmd = CLT_FindCommand(argv[0]);
		
		if( cmd == NULL ){
			fprintf(stderr, "[!] commande inconnue: '%s'\n", argv[0]);
			return;
		}
		
		if( (cmd->avail_states & CLT_CurrentState()) == 0 ){
			fprintf(stderr, "[!] la commande '%s' est invalide dans ce contexte\n", argv[0]);
			return;
		}
		
		cmd->command_fn(argc, (const char**) argv);
	}
}

