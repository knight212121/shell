#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exit_shell(CommandArgs *cmd) {
    if (cmd->argc > 2) {
        printf("Too many arguments\n");
        return;
    }
    int code = 0;
    if (cmd->argv[1]) {
        code = atoi(cmd->argv[1]);
    }
    exit(code);
}

void echo(CommandArgs *cmd) {
    for (int i = 1; i < cmd->argc; i += 1) {
        printf("%s ", cmd->argv[i]);
    }
    printf("\n");
}

void type(CommandArgs *cmd) {
    for (int i = 1; i < cmd->argc; i += 1) {
        for (int i = 0; i < command_count; i += 1) {
            if (strcmp(commands[i].name, cmd->argv[1]) == 0) {
                printf("%s is a shell builtin\n", commands[i].name);
                return;
            }
        }
    }
    printf("%s: not found\n", cmd->argv[1]);
}

ShellCommand commands[] = {
    {"echo", echo},
    {"exit", exit_shell},
    {"type", type},
};

int command_count = sizeof(commands) / sizeof(commands[0]);

int execute_builtin_command(CommandArgs *cmd) {
    for(int i = 0; i < command_count; i += 1) {
        if(strcmp(commands[i].name, cmd -> argv[0]) == 0) {
            commands[i].func(cmd);
            return 1;
        }
    }
    return 0;
}
