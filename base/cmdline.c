
#include "cmdline.h"

#include <stdlib.h>
#include <string.h>

static struct {
	int argc;
	const char **argv;
} g_cmdLine;

static bool is_option_marker(const char *v){
	assert(v != NULL);
	return (v[0] == '-');
}

void CMD_Set(int argc, const char **argv){
	assert(argv > 0 && argv != NULL);
	
	g_cmdLine.argc = argc;
	g_cmdLine.argv = argv;
}

const char * CMD_Option(const char *name, const char *default_value){
	int argc = g_cmdLine.argc;
	const char **argv = g_cmdLine.argv;
	
	// Walk through each option, ignoring the program name at argv[0]
	for(int i=1; i < argc; ++i){
		// If it doesn't start with a dash, it's not an option marker.
		if( !is_option_marker(argv[i]) )
			continue;
		
		// Now we know it's an option, but is it OUR option?
		if( strcmp(name, argv[i] + 1) != 0 )
			continue;
		
		// If the next argument isn't a value, or there is no next argument,
		// then the value is an empty string. Return NULL to signify this.
		if( i + 1 == argc || is_option_marker(argv[i + 1]) )
			return NULL;
		
		// Otherwise, just return the next value.
		return argv[i + 1];
	}
	
	// We didn't find the option, return the default value
	return default_value;
}

int CMD_OptionInt(const char *name, int default_value){
	const char *result = CMD_Option(name, NULL);
	
	if( result != NULL )
		return atoi(result);
	else
		return default_value;
}

float CMD_OptionFloat(const char *name, float default_value){
	const char *result = CMD_Option(name, NULL);
	
	if( result != NULL )
		return atof(result);
	else
		return default_value;
}

bool CMD_HasOption(const char *name){
	return (CMD_Option(name, NULL) != NULL);
}

