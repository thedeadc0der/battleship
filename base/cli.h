#ifndef BASE_CLI_H
#define BASE_CLI_H

#include "common.h"

void CLI_ReadLine(char *dst, size_t len);

typedef struct {
	const char *title;
	int value;
} CLI_MenuOption;

int CLI_Menu(const char *prompt, const CLI_MenuOption *options, size_t n);

#endif