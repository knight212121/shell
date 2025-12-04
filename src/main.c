#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void exit_shell(char* status) {
    int code = atoi(status);
    exit(code);
}

int main(int argc, char *argv[]) {
    while (1) {
        // Flush after every printf
        setbuf(stdout, NULL);

        printf("$ ");

        char command[1024];
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';

        char *rest = command, *token;
        char *executable = strtok_r(rest, " ", &rest), *args;

        while((token = strtok_r(rest, " ", &rest))) {
            args = token;
        }

        if (strcmp(executable, "exit") == 0) {
            exit_shell(args);
        }

        printf("%s: command not found\n", command);
    }

    return 0;
}
