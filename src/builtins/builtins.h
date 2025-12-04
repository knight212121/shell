#ifndef BUILTINS_H
#define BUILTINS_H

#include "../utils/common.h"

int execute_builtin_command(CommandArgs* cmd);

typedef void (*CommandFunc)(CommandArgs* cmd);

typedef struct {
    char name[10];
    CommandFunc func;
} ShellCommand;

extern ShellCommand commands[];
extern int command_count;

#endif
