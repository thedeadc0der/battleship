#ifndef BASE_CMDLINE_H
#define BASE_CMDLINE_H

#include "common.h"

void CMD_Set(int argc, const char **argv);
const char * CMD_Option(const char *name, const char *default_value);
int CMD_OptionInt(const char *name, int default_value);
float CMD_OptionFloat(const char *name, float default_value);
bool CMD_HasOption(const char *name);

#endif