#include <stdio.h>
#include <string.h>
#include "utils/common.h"
#include "input/input.h"
#include "executor/executor.h"

int main(int argc, char *argv[]) {
    while (1) {
        // Flush after every printf
        setbuf(stdout, NULL);

        printf("$ ");

        char command[1024];
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        CommandArgs* cmd = tokenize_input(command);

        execute_command(cmd);

        free_command_args(cmd);

    }

    return 0;
}
