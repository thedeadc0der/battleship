
#include "base/cli.h"

#include <stdio.h>

void CLI_ReadLine(char *dst, size_t limit){
	size_t pos = 0;
	
	fflush(stdin);
	while( pos < limit - 1 ){
		char curr = fgetc(stdin);
		
		if( curr == '\n' ){
			// XXX: Sometimes a spurious '\n' is found from the stream, despite
			//      the fflush call above. In that case, all we can do is skip
			//      it specifically.
			if( pos == 0 )
				continue;
			
			break;
		}
		
		dst[pos++] = curr;
	}
	
	dst[pos] = '\0';
	fflush(stdin);
}

int CLI_Menu(const char *prompt, const CLI_MenuOption *options, size_t n){
	puts(prompt);
	
	for(size_t i=0; i < n; ++i)
		printf(" %d: %s\n", options[i].value, options[i].title);
	
	int result = -1;
	
	for(;;) {
		int choice;
		printf("Votre choix: ");
		scanf("%d", &choice);
		fflush(stdin);
		
		for(size_t i=0; i< n; ++i){
			if( options[i].value == choice )
				return choice;
		}
		
		printf("    Choix invalide: %d\n", choice);
	}
}

