#ifndef BUILTINS_H
#define BUILTINS_H

#include "../utils/common.h"

int execute_builtin_command(CommandArgs* cmd);

typedef void (*CommandFunc)(CommandArgs* cmd);

int is_parent_builtin(char *cmd);

typedef struct {
    char name[10];
    CommandFunc func;
    int parent_builtin;
} ShellCommand;

extern ShellCommand commands[];
extern int command_count;

#ifdef _WIN32
#define PATH_LIST_SEPARATOR ";"
#else
#define PATH_LIST_SEPARATOR ":"
#endif

#endif
