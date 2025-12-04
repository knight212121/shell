#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/common.h"
#include "input/input.h"

void exit_shell(CommandArgs* cmd) {
    if (cmd -> argc > 2) {
        printf("Too many arguments\n");
        return;
    }
    int code = atoi(cmd -> argv[1]);
    exit(code);
}

void echo(CommandArgs* cmd) {
    for(int i = 1; i < cmd -> argc; i += 1) {
        printf("%s ", cmd -> argv[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    while (1) {
        // Flush after every printf
        setbuf(stdout, NULL);

        printf("$ ");

        char command[1024];
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        CommandArgs* cmd = tokenize_input(command);

        if(cmd) {
            if (strcmp(cmd -> argv[0], "exit") == 0) {
                exit_shell(cmd);
            }
            else if(strcmp(cmd -> argv[0], "echo") == 0) {
                echo(cmd);
            }
            else {
                printf("%s: command not found\n", cmd -> argv[0]);
            }
        }

    }

    return 0;
}
