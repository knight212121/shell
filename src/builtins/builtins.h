#ifndef BUILTINS_H
#define BUILTINS_H

#include "../utils/common.h"

int execute_builtin_command(CommandArgs* cmd);

typedef void (*CommandFunc)(CommandArgs* cmd);

typedef struct {
    char name[10];
    CommandFunc func;
    int is_redirect_capable;
} ShellCommand;

extern ShellCommand commands[];
extern int command_count;

#ifdef _WIN32
#define PATH_LIST_SEPARATOR ";"
#else
#define PATH_LIST_SEPARATOR ":"
#endif

#endif
